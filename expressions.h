


#ifndef NATURE_EXPRESSIONS_H
#define NATURE_EXPRESSIONS_H

#include <string>
#include "types.h"
#include "literals.h"
#include "functions.h"
#include <vector>

namespace sanema {
  struct DefineStruct {
    DefineStruct();

    std::optional<UserDefined> user_type{};
    enum DefineStructState {
      IDENTIFIER,
      FIELD_TYPE,
      FIELD_IDENTIFIER,
      FIELD_COMPLETE,
    } state{IDENTIFIER};
  };




  struct VariableEvaluation {
    std::string identifier;
  };
  struct FunctionCall;
  struct FunctionArgument;
  using Expression = std::variant<FunctionCall, VariableEvaluation, Literal>;
  struct FunctionCall {
    std::string identifier;
    std::vector<FunctionArgument> arguments;
    enum FunctionCallState {
      ARGUMENT_EXPRESION,
    } state{ARGUMENT_EXPRESION};
  };
  struct FunctionArgument {
    Expression expression;
  };



  struct DeclareVariable {
    DeclareVariable();

    std::string identifier{};
    CompleteType type_identifier{};
    std::optional<Literal> value;
    enum class DeclareVariableState {
      TYPE,
      IDENTIFIER,
      COMPLETE
    } state{DeclareVariableState::IDENTIFIER};
  };
  struct BlockInstruction;
  struct BlockOfCode {
    std::vector<BlockInstruction> instructions;
  };
   struct DefineFunction {
    std::string identifier;
    CompleteType type;
    BlockOfCode body;
    std::vector<FunctionParameter> parameters;
   std::uint64_t address{0};
     std::uint64_t id;
   std::optional<std::uint64_t> external_id{};
    enum FunctionCallState {
      IDENTIFIER,
      FUNCTION_TYPE,
      PARAMETER_MODIFIER,
      PARAMETER_TYPE,
      PARAMETER_IDENTIFIER,
      FUNCTION_BODY
    } state{IDENTIFIER};
    bool is_compatible(DefineFunction& other);

     bool operator==(const DefineFunction &rhs) const;

     bool operator!=(const DefineFunction &rhs) const;
   };
  using Instruction = std::variant<DefineStruct, DeclareVariable, DefineFunction, FunctionCall,BlockOfCode>;
  struct BlockInstruction {
    explicit BlockInstruction(const Instruction &instruction_sum);

    Instruction instruction_sum;
  };

  struct InstructionList {
    void add_expresion(Instruction const &expression);

    std::vector<Instruction> expressions;
  };
}
#endif //NATURE_EXPRESSIONS_H
