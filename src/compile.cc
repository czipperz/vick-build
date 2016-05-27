#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <atomic>
#include <fstream>

#include "compile.hh"
#include "globals.hh"
#include "print_mut.hh"

using namespace boost::filesystem;

static bool
consume_file(const path& src, const path& out, const path& dep,
             std::vector<std::string> lines) {
    if (lines.empty()) {
    invalid : {
        std::lock_guard<std::mutex> lock(print_mutex);
        fprintf(stderr, "Dependencies file `%s` is invalid.\n",
                dep.c_str());
    }
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
        } while (eit != end && eit[-1] == '\\');
        path file{it, eit};
        auto write_time_file = last_write_time(file);
        if (difftime(write_time_file, write_time_out) > 0) {
            if (IS_EXPLAIN) {
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
        return true;
    }
}

void compile(path src, path out, path dependencies) {
    if ((!HAS_ERROR || IS_KEEP_GOING) &&
        should_compile(src, out, dependencies)) {
        create_directory(out.parent_path());

        std::string combined = CXX + " -MMD -MF " +
                               dependencies.string() + " -o " +
                               out.string() + " -c " + src.string() +
                               ' ' + CXXFLAGS + ' ' + INCLUDES;

        if ((!HAS_ERROR || IS_KEEP_GOING)) {
            if (IS_VERBOSE) {
                std::lock_guard<std::mutex> lock(print_mutex);
                std::puts(combined.c_str());
            } else {
                std::string message =
                    "{" + CXX + ",Dependencies} for " + src.string();
                std::lock_guard<std::mutex> lock(print_mutex);
                std::puts(message.c_str());
            }
            if ((!HAS_ERROR || IS_KEEP_GOING) &&
                system(combined.c_str())) {
                HAS_ERROR = true;
            }
        }
    }
}
