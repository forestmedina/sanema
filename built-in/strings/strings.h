//
// Created by fores on 11/12/2023.
//
#ifndef SANEMA_STRINGS_H
#define SANEMA_STRINGS_H

#include <string>
#include <boost/algorithm/string.hpp>
namespace sanema {
  std::string replace_first(std::string input, std::string search, std::string replace);
  std::string replace_all(std::string input, std::string search, std::string replace);
  std::string concat(std::string a, std::string b);
  bool ends_with(std::string a, std::string b);
  bool starts_with(std::string a, std::string b);
  bool equal(std::string a, std::string b);


}
#endif // SANEMA_STRINGS_H
