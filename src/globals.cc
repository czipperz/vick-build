#include "globals.hh"

/* these are initialized in parse_args */
bool IS_ALL;
bool IS_TEST;
bool IS_CLEAN;
bool IS_VERBOSE;
bool IS_KEEP_GOING;
bool IS_EXPLAIN;

std::atomic<bool> HAS_ERROR;

std::string CXX = "clang++";
std::string CXXFLAGS =
    "-std=c++11 -Weverything -Wno-c++98-compat "
    "-Wno-exit-time-destructors "
    "-Wno-global-constructors -Wno-documentation-unknown-command "
    "-Wno-shadow -Wno-padded";
std::string LDFLAGS = "-lboost_regex -lncurses -lpthread";
const std::string INCLUDES = "-Iplugins -Isrc -Icatch-raw";

const std::string SRC_DIR = "src";
const std::string SRCOUT_DIR = "out";
const std::string TEST_DIR = "test";
const std::string TESTOUT_DIR = "testout";

const std::string PLUGINS_DIR = "plugins";

size_t NUM_THREADS = 0;
