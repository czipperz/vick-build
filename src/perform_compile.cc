#include <boost/filesystem.hpp>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <forward_list>
#include <algorithm>

#include "compile.hh"
#include "conversion.hh"
#include "globals.hh"
#include "perform_compile.hh"

using namespace boost::filesystem;

void PerformCompile::perform_compile(path p,
                                     path (*to_out)(const path&)) {
    const directory_iterator end;

    if (!is_directory(p)) {
        fprintf(stderr, "ERROR: Directory `%s` doesn't exist.",
                p.c_str());
        exit(1);
    }
    this->consume_directory(p, to_out);

    for (directory_iterator oiter(PLUGINS_DIR); oiter != end;
         ++oiter) {
        path dir = oiter->path();
        if (dir.c_str()[0] == '.' || !is_directory(dir)) {
            continue;
        }
        this->consume_directory(dir / p, to_out);
    }

    {
        std::lock_guard<std::mutex> _(this->mutex);
        this->done = true;
    }
    this->consumer_cond.notify_all();
}

PerformCompile::~PerformCompile() {
    this->thread_compile();
    for (auto& t : threads) {
        t.join();
    }
    if (!jobs.empty()) {
        exit(EXIT_FAILURE);
    }
}

void PerformCompile::thread_compile() {
    while (true) {
        file_group thread_job;

        {
            std::unique_lock<std::mutex> lock(mutex);
            // wait until `done || have_job`.
            consumer_cond.wait(lock, [this] {
                return done || !jobs.empty();
            });
            if (jobs.empty()) {
                if (done) {
                    return;
                } else {
                    continue;
                }
            }
            thread_job = std::move(jobs.front());
            jobs.pop_front();
        }

        if (compile(std::move(thread_job.src),
                    std::move(thread_job.obj),
                    std::move(thread_job.dep))) {
            // return;
        }
    }
}

PerformCompile::PerformCompile() {
    if (NUM_THREADS != 0) {
        threads.reserve(NUM_THREADS);
        for (size_t i = 0; i != NUM_THREADS; ++i) {
            threads.emplace_back(
                std::thread([this]() { thread_compile(); }));
        }
    }
}

void PerformCompile::consume(path src_file,
                             path (*to_out)(const path&)) {
    path object_file = to_out(src_file);
    path dependencies_file = to_dependencies(object_file);

    if (NUM_THREADS == 0) {
        if (compile(std::move(src_file), std::move(object_file),
                    std::move(dependencies_file))) {
        }
    } else {
        file_group group = {std::move(src_file),
                            std::move(object_file),
                            std::move(dependencies_file)};

        {
            std::lock_guard<std::mutex> lock(mutex);
            jobs.emplace_front(std::move(group));
        }

        consumer_cond.notify_one();
    }
}

void PerformCompile::consume_directory(path dir,
                                       path (*to_out)(const path&)) {
    if (is_directory(dir)) {
        for (directory_iterator iter(dir), end; iter != end; ++iter) {
            if (is_regular_file(*iter)) {
                auto fname = iter->path().filename();
                if (fname.c_str()[0] == '.') {
                    continue;
                }
                if (fname.extension() == ".cc") {
                    consume(iter->path(), to_out);
                }
            }
        }
    }
}
