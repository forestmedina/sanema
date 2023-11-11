//
// Created by fores on 11/3/2023.
//

#ifndef NATURE_BYTECODECOMPILER_H
#define NATURE_BYTECODECOMPILER_H

#include "Backend.h"
#include "../vm/ByteCode.h"
#include <unordered_map>
#include <parsing/FunctionCollection.h>

namespace sanema {


  class ByteCodeCompiler : public Backend {
  public:
    struct VariableEntry {
      DeclareVariable declaration;
      std::uint64_t address;
    };

    struct Scope {
      std::unordered_map<std::string,VariableEntry > local_variables;
      FunctionCollection function_collection;
      std::unordered_map<std::string, DefineStruct> types{};
      std::uint64_t context_address;
    };
    ByteCode byte_code;
    struct GeneratorsMap;
    using GeneratorFunction = void(ByteCode &byte_code, FunctionCall &function_call,std::optional<sanema::DefineFunction> const& function_definition, Scope &, GeneratorsMap &);
    struct GeneratorsMap {
      std::unordered_map<std::string, GeneratorFunction *> map;
    };

    void process(BlockOfCode &block_of_code) override;

    GeneratorsMap function_bytecode_generators;

    std::vector<Scope> scope_stack{0};
  };
}

#endif //NATURE_BYTECODECOMPILER_H
