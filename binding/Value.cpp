//
// Created by fores on 11/13/2023.
//
#include "Value.h"
std::basic_ostream<char>& operator <<(std::basic_ostream<char>& ostream, sanema::Value& value){
    std::visit([&ostream](auto final_value){
        ostream<<final_value;
    },value);
    return ostream;
};