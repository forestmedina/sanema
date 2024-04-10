


#ifndef NATURE_EXPRESSIONS_H
#define NATURE_EXPRESSIONS_H

#include <string>
#include "types.h"
#include "literals.h"
#include "functions.h"
#include <vector>

namespace sanema {
  struct IncompleteField {
    std::string identifier;
    std::optional<IncompleteType> type{};
    std::uint64_t offset;
  };
  struct DefineStruct {
    DefineStruct();
    TypeIdentifier type_id{""};
    // std::optional<std::uint64_t>  external_id;
    // std::uint64_t size;
    std::vector<IncompleteField> fields{};
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
    IncompleteType type_identifier{};
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
  struct IfStatement {
    Expression expression;
    BlockOfCode true_path;
    BlockOfCode false_path;
    enum class IfStatementState {
      EXPRESSION,
      TRUE_PATH,
      FALSE_PATH
    } state{IfStatementState::EXPRESSION};
  };
  struct ReturnStatement {
    Expression expression;
    enum class ReturnStatementState {
      EXPRESSION
    } state{ReturnStatementState::EXPRESSION};
  };

  struct DefineFunction {
    std::string identifier;
    IncompleteType type;
    BlockOfCode body;
    std::vector<FunctionParameterIncomplete> parameters;
    std::uint64_t address{0};
    std::uint64_t id;
    std::optional<std::uint64_t> external_id{};
    bool is_operator{false};
    enum FunctionCallState {
      IDENTIFIER,
      FUNCTION_TYPE,
      PARAMETER_MODIFIER,
      PARAMETER_TYPE,
      PARAMETER_IDENTIFIER,
      FUNCTION_BODY
    } state{IDENTIFIER};

  };
  struct FunctionDefinitionCompleted {
    std::string identifier;
    CompleteType type;
    BlockOfCode body;
    std::vector<FunctionParameterCompleted> parameters;
    std::uint64_t address{0};
    std::uint64_t id;
    std::optional<std::uint64_t> external_id{};
    bool is_operator{false};

    bool is_compatible(FunctionDefinitionCompleted &other);

    bool operator==(const FunctionDefinitionCompleted &rhs) const;

    bool operator!=(const FunctionDefinitionCompleted &rhs) const;
  };

  using Instruction = std::variant<DefineStruct, DeclareVariable, DefineFunction, FunctionCall, BlockOfCode, IfStatement,ReturnStatement>;

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
