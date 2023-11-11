#include "functions.h"
#include <unordered_map>

sanema::FunctionParameter::Modifier sanema::parse_modifier(std::string modifier) {
  std::unordered_map<std::string,sanema::FunctionParameter::Modifier> modifier_maps{
    {"mut",sanema::FunctionParameter::Modifier::MUTABLE},
    {"ref",sanema::FunctionParameter::Modifier::CONST},
    {"var",sanema::FunctionParameter::Modifier::VALUE}
  };
  return modifier_maps[modifier];
}

std::string sanema::serialize_modifier(sanema::FunctionParameter::Modifier modifier) {
  std::unordered_map<sanema::FunctionParameter::Modifier,std::string> modifier_maps{
    {sanema::FunctionParameter::Modifier::MUTABLE,"mut"},
    {sanema::FunctionParameter::Modifier::CONST,"ref"},
    {sanema::FunctionParameter::Modifier::VALUE,"var"}
  };
  return modifier_maps[modifier];
}

bool sanema::FunctionParameter::operator==(const sanema::FunctionParameter &rhs) const {
  return
         modifier == rhs.modifier &&
         type == rhs.type;
}

bool sanema::FunctionParameter::operator!=(const sanema::FunctionParameter &rhs) const {
  return !(rhs == *this);
}

bool sanema::FunctionParameter::is_compatible(sanema::FunctionParameter &other) {
  if(!type.has_value()||!type.has_value()) return false;
  return modifier == other.modifier &&
      is_second_type_compatible(type.value(), other.type.value());
}
