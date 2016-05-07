#include <cstdio>

#include "conversion.hh"
#include "parse_args.hh"
#include "globals.hh"
#include "perform_compile.hh"
#include "perform_clean.hh"

using namespace boost::filesystem;
int main(int argc, char** argv) {
    argc--;
    argv++;
    parse_args(argc, argv);
    if (IS_ALL) {
        /* compile source files and link executable */
        perform_compile(SRC_DIR, to_srcout);

        auto o_files = get_files(SRCOUT_DIR, ".o");
        auto command = CXX + " -o vick " + LDFLAGS;
        for (const auto& f : o_files) {
            if (f !=
                path(SRCOUT_DIR) /
                    "configuration_testing.o") {
                command += ' ';
                command += f.string();
            }
        }
        if (IS_VERBOSE) {
            std::puts(command.c_str());
        } else {
            std::puts("Linking binary");
        }
        system(command.c_str());
    }
    if (IS_TEST) {
        /* compile and run tests */
        perform_compile(TEST_DIR, to_testout);

        auto command = CXX + " -o " + TESTOUT_DIR + "/out " + LDFLAGS;
        for (const auto& dir : {SRCOUT_DIR, TESTOUT_DIR}) {
            auto o_files = get_files(dir, ".o");
            for (const auto& f : o_files) {
                if (f != path(SRCOUT_DIR) / "main.o" &&
                    f != path(SRCOUT_DIR) / "configuration.o") {
                    command += ' ';
                    command += f.string();
                }
            }
        }
        if (IS_VERBOSE) {
            std::puts(command.c_str());
        } else {
            std::puts("Linking tests");
        }
        system(command.c_str());
        std::puts("Running tests");
        auto run = path(".") / TESTOUT_DIR / "out";
        if (IS_VERBOSE) {
            std::puts(run.c_str());
        }
        system(run.c_str());
    }
    if (IS_CLEAN) {
        /* cleanup */
        perform_clean();
    }
    return 0;
}