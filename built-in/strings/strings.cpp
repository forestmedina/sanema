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

bool sanema::ends_with(const std::string &a, const std::string &b) {
  std::cout<<"Ends with "<<a<<": "<<b<<"\n";
  std::cout<<(boost::algorithm::ends_with(a,b)?"true":"false")<<"\n";
  return boost::algorithm::ends_with(a,b);
}

bool sanema::starts_with(const std::string &a, const std::string &b) {
  return boost::algorithm::starts_with(a,b);
}

bool sanema::equal(const std::string &a, const std::string &b) {
  return a==b;
}
