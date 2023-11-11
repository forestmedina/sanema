//
// Created by fores on 11/3/2023.
//

#include "Value.h"

namespace sanema {
} // sanema
std::basic_ostream<char> &operator<<(std::basic_ostream<char> &ostream, sanema::Value &value)
  {
    std::visit([&ostream](auto final_value){
        ostream<<final_value;
    },value);
    return ostream;
}


