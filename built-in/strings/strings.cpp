//
// Created by fores on 11/13/2023.
//
#include "strings.h"
#include <iostream>
std::string sanema::replace_first(const std::string &input, const std::string &search, const std::string &replace) {
  std::cout<<"input ="<<input<<"; search ="<<search<<"; replace="<<replace<<"\n";
  return boost::algorithm::replace_first_copy(input,
                                                search,
                                                replace);
}
