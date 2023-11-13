//
// Created by fores on 11/12/2023.
//

export module strings;
#include <string>
#include <boost/algorithm/string.hpp>
namespace sanema {
  std::string replace_first(std::string const &input, std::string const &search, std::string const &replace) {
    return boost::algorithm::replace_first_copy(input,
                                                search,
                                                replace);
  }
}
