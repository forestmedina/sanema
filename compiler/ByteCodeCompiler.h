//
// Created by fores on 11/3/2023.
//

#ifndef NATURE_BYTECODECOMPILER_H
#define NATURE_BYTECODECOMPILER_H

#include "Backend.h"
#include "../vm/ByteCode.h"
#include <unordered_map>
#include "common/FunctionCollection.h"
#include "common/TypeCollection.h"
#include <vm/OperandType.h>

namespace sanema {


  class ByteCodeCompiler : public Backend {
  public:
    struct VariableEntry {
      std::variant<DeclareVariable, FunctionParameter> declaration;
      std::int64_t address;
    };

    struct Scope {
      std::unordered_map<std::string, VariableEntry> local_variables;
      FunctionCollection function_collection;
     TypeCollection types{};
      address_t scope_address{0};

      void reserve_space_for_type(CompleteType const &type);
    };

    struct FuctionCallSustitution {
      std::vector<std::uint64_t> caller_addresses;// The address where the functionn is called
      std::uint64_t function_code_addres;// Address poiting to the function body
      std::uint32_t parameters_size{5};// Address poiting to the function body
      FunctionID function_id;
    };
    std::vector<FuctionCallSustitution> function_call_sustitutions{};
    std::vector<FunctionID> pendind_to_generate_functions;

    ByteCode byte_code;
    struct GeneratorsMap;
    using GeneratorFunction = void(ByteCode &byte_code,
                                   std::optional<sanema::DefineFunction> const &function_definition);
    struct GeneratorsMap {
      std::unordered_map<std::string, GeneratorFunction *> map;
    };

    void process(BlockOfCode &block_of_code, FunctionCollection &built_in_functions,TypeCollection& external_types) override;
    std::uint32_t generate_block(BlockOfCode &block_of_code, FunctionCollection &built_in_functions,TypeCollection& external_types) ;

    GeneratorsMap function_bytecode_generators;

    std::vector<Scope> scope_stack{0};
  };
}

#endif //NATURE_BYTECODECOMPILER_H
