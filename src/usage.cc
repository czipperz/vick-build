#include <stdio.h>

#include "usage.hh"
#include "globals.hh"

void usage(const char* progname) {
    fprintf(stderr,
            "Usage: %s [configuration] ... [targets]\n"
            "\n"
            "Configuration:\n"
            "    NUM_THREADS  Specify the number of threads that will be used for\n"
            "                 various tasks.  Defaults to 0.\n"
            "    CXX          The compiler that will be used for `*.cc` files.\n"
            "    CXXFLAGS     The flags that will be given to CXX when compiling\n"
            "                 `*.cc` files.\n"
            "    LDFLAGS      The flags that will be given to CXX when linking\n"
            "                 the program.\n"
            "\n"
            , progname);
    fprintf(stderr,
            "Targets:\n"
            "    clean        Remove the `%s` and `%s` directories.\n"
            "    test         Runs `build`, then compiles the files in the `%s`\n"
            "                 directories into the respective `%s` directories.\n"
            "                 Then it links all the testing and source object\n"
            "                 files together, running the resulting binary.\n"
            "    build        Compile the files in the `%s` directories into the\n"
            "                 respective `%s` direcories.  Then it links all the\n"
            "                 source object files together.\n"
            "    help         Show usage and exit.\n",
            TEST_DIR.c_str(), TESTOUT_DIR.c_str(), TEST_DIR.c_str(),
            TESTOUT_DIR.c_str(), SRC_DIR.c_str(), SRCOUT_DIR.c_str());
}