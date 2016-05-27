#ifndef HEADER_GUARD_PERFORM_TESTS_H
#define HEADER_GUARD_PERFORM_TESTS_H

#include <boost/filesystem.hpp>
#include <condition_variable>
#include <forward_list>
#include <mutex>
#include <thread>

/// Unfortunately we have to export this because `PerformCompile` uses
/// it.
class file_group {
public:
    file_group() = default;
    file_group(boost::filesystem::path src,
               boost::filesystem::path obj,
               boost::filesystem::path dep)
        : src(src)
        , obj(obj)
        , dep(dep) {}

    /// The source file
    boost::filesystem::path src;
    /// The object file
    boost::filesystem::path obj;
    /// The dependencies file
    boost::filesystem::path dep;

    // ensure only ever move, as intended.
    file_group& operator=(const file_group&) = delete;
    file_group& operator=(file_group&&) = default;
    file_group(const file_group&) = delete;
    file_group(file_group&&) = default;
};

/// Export class so only spin up and join threads once.
class PerformCompile {
public:
    /// \brief Add all source files in `dir` to be compiled.
    void perform_compile(boost::filesystem::path dir,
                         boost::filesystem::path (*to_out)(
                             const boost::filesystem::path&));

    /// \brief Join all the threads.
    ~PerformCompile();

    PerformCompile();

private:
    PerformCompile(const PerformCompile&) = delete;
    PerformCompile(PerformCompile&&) = delete;

    void thread_compile();
    void consume_directory(boost::filesystem::path src_file,
                           boost::filesystem::path (*to_out)(
                               const boost::filesystem::path&));
    void consume(boost::filesystem::path src_file,
                 boost::filesystem::path (*to_out)(
                     const boost::filesystem::path&));

    std::forward_list<file_group> jobs;
    std::mutex mutex;
    std::condition_variable consumer_cond;
    std::vector<std::thread> threads;
    bool done = false;
};

#endif
