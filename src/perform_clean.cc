#include "perform_clean.hh"
#include "globals.hh"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;
using namespace boost::system;

void perform_clean() {
    uintmax_t total = 0;
    error_code ec;
    total += remove_all(SRCOUT_DIR, ec);
    total += remove_all(TESTOUT_DIR, ec);
    for (auto plug = directory_iterator(PLUGINS_DIR);
         plug != directory_iterator(); ++plug) {
        if (exists(*plug)) {
            total += remove_all(*plug / SRCOUT_DIR, ec);
            total += remove_all(*plug / TESTOUT_DIR, ec);
        }
    }
    puts((std::string("Deleted ") +
          std::to_string(total) + " files").c_str());
}
