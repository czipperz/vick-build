#include "conversion.hh"
#include "globals.hh"
#include "print_mut.hh"

using namespace boost::filesystem;

path to_srcout(const path& source_file) {
    return source_file.parent_path().parent_path() / SRCOUT_DIR /
           change_extension(source_file.filename(), "o");
}

path to_testout(const path& test_file) {
    return test_file.parent_path().parent_path() / TESTOUT_DIR /
           change_extension(test_file.filename(), "o");
}

path to_dependencies(const path& object_file) {
    return change_extension(object_file, "m");
}

std::vector<path> get_files(const path& p, const std::string& ext) {
    std::vector<path> vec;
    auto scan_dir = [&](const path& dir) {
        if (is_directory(dir)) {
            std::for_each(directory_iterator(dir),
                          directory_iterator(),
                          [&](const path& file) {
                              if (is_regular_file(file) &&
                                  file.c_str()[0] != '.' &&
                                  file.extension() == ext) {
                                  vec.push_back(file);
                              }
                          });
        }
    };

    if (!is_directory(p)) {
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            fprintf(stderr, "ERROR: Directory `%s` doesn't exist.",
                    p.c_str());
        }
        exit(EXIT_FAILURE);
    }
    scan_dir(p);

    std::for_each(directory_iterator(PLUGINS_DIR),
                  directory_iterator(),
                  [&p, &scan_dir, &vec](const path& plug) {
                      if (plug.c_str()[0] != '.' &&
                          is_directory(plug)) {
                          scan_dir(plug / p);
                      }
                  });
    return vec;
}
