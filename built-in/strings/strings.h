//
// Created by fores on 11/12/2023.
//
#ifndef SANEMA_STRINGS_H
#define SANEMA_STRINGS_H

#include <string>
#include <boost/algorithm/string.hpp>
namespace sanema {
  std::string replace_first(std::string const &input, std::string const &search, std::string const &replace);
  std::string replace_all(std::string const &input, std::string const &search, std::string const &replace);
  std::string concat(std::string const &a, std::string const &b);
  bool ends_with(std::string const &a, std::string const &b);
  bool starts_with(std::string const &a, std::string const &b);
  bool equal(std::string const &a, std::string const &b);


}
#endif // SANEMA_STRINGS_H
