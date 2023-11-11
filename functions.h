


#ifndef NATURE_FUNCTIONS_H
#define NATURE_FUNCTIONS_H
#include <string>
#include <vector>
#include <optional>
#include "types.h"
namespace sanema
{
  struct   FunctionParameter
  {
    std::string identifier{};
    enum class Modifier{
      VALUE,
      MUTABLE,
      CONST
    }modifier;
    std::optional<CompleteType>  type{};
    bool is_compatible(FunctionParameter& other);
    bool operator==(const FunctionParameter &rhs) const;

    bool operator!=(const FunctionParameter &rhs) const;
  };

  struct Function
  {
    std::string identifier;
    std::vector<FunctionParameter> parameter;
  };
  FunctionParameter::Modifier  parse_modifier(std::string modifier);
  std::string serialize_modifier(FunctionParameter::Modifier modifier);

}

#endif //NATURE_FUNCTIONS_H
