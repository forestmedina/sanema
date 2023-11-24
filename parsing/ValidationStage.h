//
// Created by fores on 11/4/2023.
//

#ifndef NATURE_VALIDATIONSTAGE_H
#define NATURE_VALIDATIONSTAGE_H
#include <string>
#include <unordered_map>
#include "../expressions.h"
#include "common/FunctionCollection.h"
namespace sanema {
  class ValidationStage {
  public:
    struct Scope {
      FunctionCollection function_definitions{};
      std::unordered_map<std::string, DefineStruct> types{};
      std::unordered_map<std::string, DeclareVariable> variables{};
      int current_instruction;
    };
    void process(BlockOfCode &block_of_code);

  };
}


#endif //NATURE_VALIDATIONSTAGE_H
