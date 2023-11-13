//
// Created by fores on 11/3/2023.
//

#ifndef NATURE_VALUE_H
#define NATURE_VALUE_H
#include <variant>
#include <string>
#include <iostream>
export module Value;
namespace sanema {

 using Value=std::variant<
  std::uint8_t,
  std::uint16_t,
  std::uint32_t,
  std::uint64_t,
  std::int8_t,
  std::int16_t,
  std::int32_t,
  std::int64_t,
  float,
  double,
  bool,
  std::string>;

} // sanema

std::basic_ostream<char>& operator <<(std::basic_ostream<char>& ostream, sanema::Value& value){
    std::visit([&ostream](auto final_value){
        ostream<<final_value;
    },value);
    return ostream;
};
#endif //NATURE_VALUE_H
