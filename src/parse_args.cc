#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.hh"
#include "parse_args.hh"
#include "usage.hh"

static int myatoi(const char* str, const char* s) {
    int val = 0;
    if (!*s) {
        fprintf(stderr, "Error: Must specify number of threads.  "
                        "Context:\n%s\n",
                str);
        exit(EXIT_FAILURE);
    }
    /* can skip first check since done in if statement above */
    do {
        if (*s >= '0' && *s <= '9') {
            val = val * 10 + (*s++ - '0');
        } else {
            fprintf(stderr, "Non number character (%c) found "
                            "where expected number.  Context:\n%s\n",
                    *s, str);
            exit(EXIT_FAILURE);
        }
    } while (*s);
    return val;
}

void parse_args(int argc, char** argv) {
    bool all = false, test = false, clean = false, verbose = false,
         num_threads = false, keep_going = false, explain = false;
    bool implicit_all = true;
    bool exit_at_fin = false;

    if (argc == 0) {
        implicit_all = true;
    }
    for (int argnum = 0; argnum != argc; ++argnum) {
        const char* const str = argv[argnum];
        if (strncmp(str, "CXX=", 4) == 0) {
            /* CXX=<compiler> */
            if (str[4] == 0) {
                fputs("Error: CXX cannot be empty\n", stderr);
                exit(EXIT_FAILURE);
            }
            CXX = str + 4;
        } else if (strcmp(str, "CXX") == 0) {
            printf("CXX: `%s`\n", CXX.c_str());
            exit_at_fin = true;


        } else if (strncmp(str, "NUM_THREADS=", 12) == 0) {
            int val = myatoi(str, str + 12);
            if (clean) {
                fputs("Warning: Giving NUM_THREADS to `clean` is "
                      "currently useless.  This may change in future "
                      "versions.\n",
                      stderr);
            }
            num_threads = true;
            NUM_THREADS = val;
        } else if (strcmp(str, "NUM_THREADS") == 0) {
            printf("NUM_THREADS: `%zu`\n", NUM_THREADS);
            exit_at_fin = true;


        } else if (strncmp(str, "CXXFLAGS=", 9) == 0) {
            /* CXXFLAGS=<new CXXFLAGS> */
            CXXFLAGS = str + 9;
        } else if (strncmp(str, "CXXFLAGS+=", 10) == 0) {
            /* CXXFLAGS+=<extra CXXFLAGS> */
            if (str[10] == 0) {
                fputs("Error: cannot append empty string to CXX\n",
                      stderr);
                exit(EXIT_FAILURE);
            }
            CXXFLAGS += str + 10;
        } else if (strcmp(str, "CXXFLAGS") == 0) {
            /* CXXFLAGS : print CXXFLAGS */
            printf("CXXFLAGS: `%s`\n", CXXFLAGS.c_str());
            exit_at_fin = true;


        } else if (strncmp(str, "LDFLAGS=", 8) == 0) {
            CXXFLAGS = str + 8;
        } else if (strcmp(str, "LDFLAGS") == 0) {
            printf("CXXFLAGS: `%s`\n", CXXFLAGS.c_str());
            exit_at_fin = true;


        } else if (strcmp(str, "-v") == 0 ||
                   strcmp(str, "--verbose") == 0) {
            verbose = true;


        } else if (strcmp(str, "-e") == 0 ||
                   strcmp(str, "--explain") == 0) {
            explain = true;


        } else if (strcmp(str, "-k") == 0 ||
                   strcmp(str, "--keep-going") == 0) {
            keep_going = true;


        } else if (strcmp(str, "test") == 0) {
            if (clean) {
                fputs("ERROR: Cannot run `test` whilst running "
                      "`clean`.\n",
                      stderr);
                exit(EXIT_FAILURE);
            }
            if (all) {
                fputs("Warning: `test` implies `all`, specifying "
                      "`all` and `test` is redundant.\n",
                      stderr);
            }
            all = true;
            test = true;


        } else if (strcmp(str, "all") == 0) {
            if (clean) {
                fputs("ERROR: Cannot run `test` whilst running "
                      "`clean`.\n",
                      stderr);
                exit(EXIT_FAILURE);
            }
            if (test) {
                fputs("Warning: `test` implies `all`, specifying "
                      "`all` and `test` is redundant.\n",
                      stderr);
            }
            all = true;


        } else if (strcmp(str, "clean") == 0) {
            implicit_all = false;
            if (all || test) {
                fputs("ERROR: Cannot run `clean` whilst running "
                      "`all` and/or `test`.\n",
                      stderr);
                exit(EXIT_FAILURE);
            }
            if (num_threads && NUM_THREADS != 0) {
                fputs("Warning: Giving NUM_THREADS to `clean` is "
                      "currently useless.  This may change in future "
                      "versions.\n",
                      stderr);
            }
            clean = true;


        } else if (strcmp(str, "help") == 0) {
            usage(argv[-1]);
            exit(EXIT_SUCCESS);


        } else {
            fprintf(stderr, "Unable to parse argument: %s\n", str);
            usage(argv[-1]);
            exit(EXIT_FAILURE);
        }
    }
    if (exit_at_fin) {
        fprintf(stderr,
                "Specified a printing operation so exiting.\n");
        exit(EXIT_SUCCESS);
    }
    IS_ALL = implicit_all || all;
    IS_TEST = test;
    IS_CLEAN = clean;
    IS_VERBOSE = verbose;
    IS_KEEP_GOING = keep_going;
    IS_EXPLAIN = explain;
}
