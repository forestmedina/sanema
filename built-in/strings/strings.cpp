//
// Created by fores on 11/13/2023.
//
#include "strings.h"
#include <iostream>
std::string sanema::replace_first(const std::string &input, const std::string &search, const std::string &replace) {
  return boost::algorithm::replace_first_copy(input,
                                                search,
                                                replace);
}
std::string sanema::replace_all(const std::string &input, const std::string &search, const std::string &replace) {
  return boost::algorithm::replace_all_copy(input,
                                                search,
                                                replace);
}
std::string sanema::concat(std::string const &a, std::string const &b) {
   return a+b;
}