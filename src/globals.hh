#ifndef HEADER_GUARD_GLOBALS_H
#define HEADER_GUARD_GLOBALS_H

#include <string>

/**
 * `IS_ALL` is `true` if there are no targets or the target given is
 * `test` or `build`.
 *
 * If `IS_TEST`, `IS_ALL` must be `true`.
 */
extern bool IS_ALL;

/**
 * `IS_TEST` is `true` if the target given is `test`.
 *
 * If `IS_TEST`, `IS_ALL` must be `true`.
 */
extern bool IS_TEST;

/**
 * `IS_CLEAN` is `true` if the target given is `clean`.
 *
 * If `IS_CLEAN`, `IS_ALL` must be `false` and `IS_TEST` must be
 * false.
 */
extern bool IS_CLEAN;

/**
 * If `IS_VERBOSE` is `true` then compiling shall output the full
 * command, otherwise it shall output a short summary.
 */
extern bool IS_VERBOSE;

/**
 * This is the compiler to be used.  Defaults to "g++".
 */
extern std::string CXX;

/**
 * This is the flags to be given to the compiler.
 *
 * @see CXX
 */
extern std::string CXXFLAGS;

/**
 * This is the flags to be given to the compiler at link time.
 *
 * @see CXX
 */
extern std::string LDFLAGS;

/**
 * Immutable flags, denoting directories to include, given to the
 * compiler.
 *
 * @see CXX
 */
extern const std::string INCLUDES;

/**
 * This is the directory where source files are located.
 */
extern const std::string SRC_DIR;

/**
 * This is the directory where source files will be compiled to.
 */
extern const std::string SRCOUT_DIR;

/**
 * This is the directory where testing source files are located.
 */
extern const std::string TEST_DIR;

/**
 * This is the directory where testing source files will be compiled
 * to.
 */
extern const std::string TESTOUT_DIR;

/**
 * This is the directory where the plugins are located.
 */
extern const std::string PLUGINS_DIR;

/**
 * This controlls the amount of threads to be used for building the
 * project.
 *
 * If NUM_THREADS == 0, it will use only the main thread.
 */
extern size_t NUM_THREADS;

#endif
