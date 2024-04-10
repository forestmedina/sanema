#include "functions.h"
#include <unordered_map>

sanema::FunctionParameterIncomplete::Modifier sanema::parse_modifier(std::string modifier) {
  std::unordered_map<std::string,sanema::FunctionParameterIncomplete::Modifier> modifier_maps{
    {"mut",sanema::FunctionParameterIncomplete::Modifier::MUTABLE},
    {"ref",sanema::FunctionParameterIncomplete::Modifier::CONST},
    {"var",sanema::FunctionParameterIncomplete::Modifier::VALUE}
  };
  return modifier_maps[modifier];
}

std::string sanema::serialize_modifier(sanema::FunctionParameterIncomplete::Modifier modifier) {
  std::unordered_map<sanema::FunctionParameterIncomplete::Modifier,std::string> modifier_maps{
    {sanema::FunctionParameterIncomplete::Modifier::MUTABLE,"mut"},
    {sanema::FunctionParameterIncomplete::Modifier::CONST,"ref"},
    {sanema::FunctionParameterIncomplete::Modifier::VALUE,"var"}
  };
  return modifier_maps[modifier];
}

bool sanema::FunctionParameterCompleted::operator==(const sanema::FunctionParameterCompleted &rhs) const {
  return
         modifier == rhs.modifier &&
         type == rhs.type;
}

bool sanema::FunctionParameterCompleted::operator!=(const sanema::FunctionParameterCompleted &rhs) const {
  return !(rhs == *this);
}

bool sanema::FunctionParameterCompleted::is_compatible(sanema::FunctionParameterCompleted &other) {
  if(!type.has_value()||!other.type.has_value()) return false;
  return is_second_type_compatible(type.value(), other.type.value());
}
