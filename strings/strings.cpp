//
// Created by fores on 11/13/2023.
//
#include "strings.h"

std::string sanema::replace_first(const std::string &input, const std::string &search, const std::string &replace) {
  return boost::algorithm::replace_first_copy(input,
                                                search,
                                                replace);
}
