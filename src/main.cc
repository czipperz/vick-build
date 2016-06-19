#include <cstdio>

#include "conversion.hh"
#include "globals.hh"
#include "parse_args.hh"
#include "perform_clean.hh"
#include "perform_compile.hh"

using namespace boost::filesystem;
int main(int argc, char** argv) {
    argc--;
    argv++;
    parse_args(argc, argv);

    if (IS_ALL || IS_TEST) {
        PerformCompile pc;
        if (IS_ALL) {
            // add sources to be compiled
            pc.perform_compile(SRC_DIR, to_srcout);
        }
        if (IS_TEST) {
            // add tests to be compiled
            pc.perform_compile(TEST_DIR, to_testout);
        }
        // compile added files (destructor)
    }

    if (HAS_ERROR) {
        exit(EXIT_FAILURE);
    }

    if (IS_ALL) {
        // link executable
        auto o_files = get_files(SRCOUT_DIR, ".o");
        auto command = CXX + " -o vick " + LDFLAGS;
        for (const auto& f : o_files) {
            if (f != path(SRCOUT_DIR) / "configuration_testing.o") {
                command += ' ';
                command += f.string();
            }
        }
        if (IS_VERBOSE) {
            std::puts(command.c_str());
        } else {
            std::puts("Linking binary");
        }
        std::fflush(stdout);
        if (system(command.c_str())) {
            exit(EXIT_FAILURE);
        }
    }
    if (IS_TEST) {
        // link tests
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
        std::fflush(stdout);
        if (system(command.c_str())) {
            exit(EXIT_FAILURE);
        }
        std::puts("Running tests");
        auto run = path(".") / TESTOUT_DIR / "out";
        if (IS_VERBOSE) {
            std::puts(run.c_str());
        }
        std::fflush(stdout);
        if (system(run.c_str())) {
            exit(EXIT_FAILURE);
        }
    }
    if (IS_CLEAN) {
        /* cleanup */
        perform_clean();
    }
    return 0;
}
