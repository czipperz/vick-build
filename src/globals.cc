#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "globals.hh"

/* these three are initialized in parse_args */
bool IS_ALL;
bool IS_TEST;
bool IS_CLEAN;
bool IS_VERBOSE;

std::string CXX = "g++";
std::string CXXFLAGS =
    "-std=c++11 -Wall -Wextra -Wold-style-cast -Wnon-virtual-dtor "
    "-Wnarrowing -Wdelete-non-virtual-dtor -Wctor-dtor-privacy "
    "-Woverloaded-virtual -Wsign-promo";
std::string LDFLAGS = "-lboost_regex -lncurses -lpthread";

const std::string SRC_DIR = "src";
const std::string SRCOUT_DIR = "out";
const std::string TEST_DIR = "test";
const std::string TESTOUT_DIR = "testout";

const std::string PLUGINS_DIR = "plugins";

size_t NUM_THREADS = 0;