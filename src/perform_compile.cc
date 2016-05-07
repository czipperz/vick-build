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
struct file_group {
    file_group() = default;
    file_group(path src, path obj, path dep)
        : src(src), obj(obj), dep(dep) {}

    /// The source file
    path src;
    /// The object file
    path obj;
    /// The dependencies file
    path dep;

    // ensure only ever move, as intended.
    file_group& operator=(const file_group&) = delete;
    file_group& operator=(file_group&&) = default;
    file_group(const file_group&) = delete;
    file_group(file_group&&) = default;
};

struct Compile {
    ~Compile() {
        for (auto& t : threads) {
            t.join();
        }
        if (!jobs.empty()) {
            exit(EXIT_FAILURE);
        }
    }

    void thread_compile() {
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
                return;
            }
        }
    }

    void consume(path src_file, path (*to_out)(const path&)) {
        static bool first_time = true;

        if (first_time) {
            first_time = false;
            if (NUM_THREADS != 0) {
                threads.reserve(NUM_THREADS);
                for (size_t i = 0; i != NUM_THREADS; ++i) {
                    threads.emplace_back(
                        std::thread([this]() { thread_compile(); }));
                }
            }
        }

        path object_file = to_out(src_file);
        path dependencies_file = to_dependencies(object_file);

        if (NUM_THREADS == 0) {
            if (compile(std::move(src_file),
                        std::move(object_file),
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

    void consume_directory(path dir, path (*to_out)(const path&)) {
        if (is_directory(dir)) {
            for (directory_iterator iter(dir), end;
                iter != end; ++iter) {
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

    std::forward_list<file_group> jobs;
    std::mutex mutex;
    std::condition_variable consumer_cond;
    bool done = false;
    std::vector<std::thread> threads;
};

void perform_compile(path p, path (*to_out)(const path&)) {
    Compile c;
    const directory_iterator end;

    if (!is_directory(p)) {
        fprintf(stderr, "ERROR: Directory `%s` doesn't exist.",
                p.c_str());
        exit(1);
    }
    c.consume_directory(p, to_out);

    for (directory_iterator oiter(PLUGINS_DIR); oiter != end;
         ++oiter) {
        path dir = oiter->path();
        if (dir.c_str()[0] == '.' || !is_directory(dir)) {
            continue;
        }
        c.consume_directory(dir / p, to_out);
    }

    {
        std::lock_guard<std::mutex> _(c.mutex);
        c.done = true;
    }
    c.consumer_cond.notify_all();
    c.thread_compile();
}
