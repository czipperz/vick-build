#ifndef HEADER_GUARD_COMPILE_H
#define HEADER_GUARD_COMPILE_H

#include <boost/filesystem.hpp>
void compile(boost::filesystem::path in, boost::filesystem::path out,
             boost::filesystem::path dependencies);

#endif
