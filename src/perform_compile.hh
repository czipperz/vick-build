#ifndef HEADER_GUARD_PERFORM_TESTS_H
#define HEADER_GUARD_PERFORM_TESTS_H

#include <boost/filesystem.hpp>

void perform_compile(boost::filesystem::path dir,
                     boost::filesystem::path (*to_out)(
                         const boost::filesystem::path&));

#endif
