#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "print_mut.hh"
#include "compile.hh"
#include "globals.hh"

std::atomic<bool> force_finish(false);

using namespace boost::filesystem;
static void
generate_dependencies(const path& src, const path& dependencies) {
    std::string command = CXX + " -std=c++11 -MM -MF " +
                          dependencies.string() + ' ' + src.string();
    if (IS_VERBOSE) {
        std::lock_guard<std::mutex> lock(print_mutex);
        puts(command.c_str());
    } else {
        std::string message = "Dependencies for " + src.string();
        std::lock_guard<std::mutex> lock(print_mutex);
        puts(message.c_str());
    }
    if (system(command.c_str())) {
        force_finish = true;
    }
}

static bool
consume_file(const path& src, const path& out, const path& dep,
             std::vector<std::string> lines) {
    if (lines.empty()) {
    invalid:
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            fprintf(stderr, "Dependencies file `%s` is invalid.\n",
                    dep.c_str());
        }
        generate_dependencies(src, dep);
        return true;
    }

    auto write_time_out = last_write_time(out);
    std::size_t line = 0;
    auto it = lines[line].begin();
    auto end = lines[line].end();

    // skip FNAME:
    it = std::find(it, end, ':');
    if (it == end) {
        // this only happens if invalid dependencies file, so regen.
        goto invalid;
    }
    ++it;
    do {
        ++it;
        if (it >= end || (*it == '\\' && it + 1 == end)) {
            ++line;
            if (line == lines.size()) {
                return false;
            }
            it = lines[line].begin();
            end = lines[line].end();
            if (it + 1 >= end) {
                // empty line
                goto invalid;
            }
            it = std::find_if_not(it, end, [](char ch) {
                return std::isspace(ch);
            });
            if (it == end) {
                goto invalid;
            }
        }
        auto eit = it;
        do {
            eit = std::find_if(eit, end, [](char ch) {
                return std::isspace(ch);
            });
            // if space in file path, uses `\ ` format
            // if (eit == end || eit[-1] != '\\') {
            //     break;
            // }
        } while (eit != end && eit[-1] != '\\');
        path file{it, eit};
        auto write_time_file = last_write_time(file);
        if (difftime(write_time_file, write_time_out) > 0) {
            {
                std::string message = "Compile " + src.string() +
                                      " because of " + file.string();
                std::lock_guard<std::mutex> lock(print_mutex);
                std::puts(file.c_str());
            }
            return true;
        }
        it = eit;
    } while (1);
}

static bool should_compile(const path& src, const path& out,
                           const path& dependencies) {
    if (!exists(out)) {
        return true;
    }
    std::ifstream fdependencies(dependencies.string());
    if (fdependencies) {
        std::vector<std::string> lines;
        std::string temp;
        while (std::getline(fdependencies, temp)) {
            lines.emplace_back(std::move(temp));
        }
        return consume_file(src, out, dependencies, std::move(lines));
    } else {
        generate_dependencies(src, dependencies);
        return true;
    }
}

bool compile(path src, path out, path dependencies) {
    if (!force_finish && should_compile(src, out, dependencies)) {
        create_directory(out.parent_path());

        std::string combined = CXX + " -o " + out.string() + " -c " +
                               src.string() + ' ' + CXXFLAGS;

        if (!force_finish) {
            if (IS_VERBOSE) {
                std::lock_guard<std::mutex> lock(print_mutex);
                std::puts(combined.c_str());
            } else {
                std::string message = CXX + " for " + src.string();
                std::lock_guard<std::mutex> lock(print_mutex);
                std::puts(message.c_str());
            }
            if (system(combined.c_str())) {
                force_finish = true;
            }
        }
    }
    return force_finish;
}
