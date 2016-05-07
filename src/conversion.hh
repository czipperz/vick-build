#ifndef HEADER_GUARD_NEW_CONVERSION_H
#define HEADER_GUARD_NEW_CONVERSION_H

#include <boost/filesystem.hpp>

boost::filesystem::path
to_srcout(const boost::filesystem::path& source_file);

boost::filesystem::path
to_testout(const boost::filesystem::path& test_file);

boost::filesystem::path
to_dependencies(const boost::filesystem::path& object_file);

std::vector<boost::filesystem::path>
get_files(const boost::filesystem::path& p,
          const std::string& extension);

#endif
