


#ifndef NATURE_FUNCTIONS_H
#define NATURE_FUNCTIONS_H
#include <string>
#include <vector>
#include <optional>
#include "types.h"
namespace sanema
{
  struct   FunctionParameterIncomplete
  {
    std::string identifier{};
    enum class  Modifier{
      VALUE,
      MUTABLE,
      CONST
    };
    Modifier modifier;
    std::optional<IncompleteType>  type{};

  };

  struct   FunctionParameterCompleted
  {
    std::string identifier{};
    enum class  Modifier{
      VALUE,
      MUTABLE,
      CONST
    };
    Modifier modifier;
    std::optional<CompleteType>  type{};
    bool is_compatible(FunctionParameterCompleted& other);
    bool operator==(const FunctionParameterCompleted &rhs) const;

    bool operator!=(const FunctionParameterCompleted &rhs) const;

  };

  struct Function
  {
    std::string identifier;
    std::vector<FunctionParameterIncomplete> parameter;
  };
  FunctionParameterIncomplete::Modifier  parse_modifier(std::string modifier);
  std::string serialize_modifier(FunctionParameterIncomplete::Modifier modifier);

}

#endif //NATURE_FUNCTIONS_H
