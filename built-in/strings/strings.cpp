//
// Created by fores on 11/13/2023.
//
#include "strings.h"
#include <iostream>
#include <format>
std::string sanema::replace_first(std::string input,  std::string search,  std::string replace) {
  return boost::algorithm::replace_first_copy(input,
                                                search,
                                                replace);
}
std::string sanema::replace_all(std::string input,  std::string search,  std::string replace) {
  return boost::algorithm::replace_all_copy(input,
                                                search,
                                                replace);
}
std::string sanema::concat(std::string  a, std::string  b) {
   return a+b;
}

bool sanema::ends_with( std::string a,  std::string b) {
  return boost::algorithm::ends_with(a,b);
}

bool sanema::starts_with( std::string a,  std::string b) {
  return boost::algorithm::starts_with(a,b);
}

bool sanema::equal( std::string a,  std::string b) {
  return a==b;
}
