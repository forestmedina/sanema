//
// Created by fores on 11/3/2023.
//

#include "ByteCodeCompiler.h"
#include <vector>
#include <util/lambda_visitor.hpp>
#include <built-in/built_in_functions.h>
#include "../vm/opcodes.h"
#include <format>
#include <assert.h>

std::optional<sanema::DefineFunction> get_function_definition(sanema::FunctionCall &function_call,
                                                              sanema::ByteCodeCompiler::Scope &scope);

std::optional<sanema::CompleteType>
get_expression_type(sanema::Expression &expression, sanema::ByteCodeCompiler::Scope &scope) {
  return match(expression,
               [&](sanema::FunctionCall &function_call_nested) -> std::optional<sanema::CompleteType> {
                 auto function_definition_nested = get_function_definition(function_call_nested,
                                                                           scope);
                 if (function_definition_nested.has_value()) {
                   return function_definition_nested.value().type;
                 } else {
                   return {};
                 }
               },
               [&](sanema::VariableEvaluation &variable) -> std::optional<sanema::CompleteType> {
                 auto context = scope;
                 if (context.local_variables.count(variable.identifier) > 0) {
                   return context.local_variables.at(variable.identifier).declaration.type_identifier;
                 }
                 return {};
               },
               [&](sanema::Literal &literal) -> std::optional<sanema::CompleteType> {
                 return match(literal,
                              [&](sanema::LiteralSInt64 int_64) -> sanema::CompleteType {
                                return sanema::Integer{64};
                              },
                              [](sanema::LiteralSInt32 int_32) -> sanema::CompleteType {
                                return sanema::Integer{32};
                              },
                              [](sanema::LiteralSInt16 int_16) -> sanema::CompleteType {
                                return sanema::Integer{16};
                              },
                              [](sanema::LiteralSInt8 int_8) -> sanema::CompleteType {
                                return sanema::Integer{8};
                              },
                              [](sanema::LiteralBoolean boolean) -> sanema::CompleteType {
                                return sanema::Boolean{};

                              },
                              [](sanema::LiteralFloat a_float) -> sanema::CompleteType {
                                return sanema::Float{};
                              },
                              [](sanema::LiteralDouble a_double) -> sanema::CompleteType {
                                return sanema::Double{};
                              },
                              [](sanema::LiteralString &string) -> sanema::CompleteType {
                                return sanema::String{};
                              }
                             );
               }
              );
}

std::optional<sanema::DefineFunction> get_function_definition(sanema::FunctionCall &function_call,
                                                              sanema::ByteCodeCompiler::Scope &scope) {
  sanema::DefineFunction function_definition;
  function_definition.identifier = function_call.identifier;
  for (auto &argument: function_call.arguments) {
    auto type = get_expression_type(argument.expression,
                                    scope);
    if (!type.has_value()) return {};
    function_definition.parameter.emplace_back(sanema::FunctionParameter{"", {}, type.value()});
  }
  auto found_function = find_function(scope.function_collection,
                                      function_definition);
  return found_function;
}

void generate_if(ByteCode &byte_code, sanema::FunctionCall &function_call,
                 sanema::ByteCodeCompiler::Scope &context_frame_aux,
                 sanema::ByteCodeCompiler::GeneratorsMap &generator_map) {
  byte_code.write(OPCODE::OP_JUMP_IF_FALSE);
}

template<class T>
T convert_number_literal(sanema::Literal literal) {
  return match(literal,
               [&](sanema::LiteralString &string) {
                 return T{};
               },
               [&](auto &literal_value) {
                 return (T) literal_value.value;
               }
              );
}

void
generate_push_literal(ByteCode &byte_code, sanema::Literal &literal, sanema::ByteCodeCompiler::Scope &context_frame_aux,
                      sanema::ByteCodeCompiler::GeneratorsMap &generator_map, sanema::CompleteType const &type) {
  match(literal,
        [&byte_code](sanema::LiteralSInt64 int_64) {
          byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
          byte_code.write(int_64.value);
        },
        [&byte_code](sanema::LiteralSInt32 int_32) {
          byte_code.write(OPCODE::OP_PUSH_SINT32_CONST);
          byte_code.write(int_32.value);
        },
        [&byte_code](sanema::LiteralSInt16 int_16) {
          byte_code.write(OPCODE::OP_PUSH_SINT16_CONST);
          byte_code.write(int_16.value);
        },
        [&byte_code](sanema::LiteralSInt8 int_8) {
          byte_code.write(OPCODE::OP_PUSH_SINT8_CONST);
          byte_code.write(int_8.value);
        },
        [&byte_code](sanema::LiteralBoolean boolean) {
          byte_code.write(boolean.value ? OPCODE::OP_TRUE : OPCODE::OP_FALSE);

        },
        [&byte_code](sanema::LiteralFloat a_float) {
          byte_code.write(OPCODE::OP_PUSH_FLOAT_CONST);
          byte_code.write(a_float.value);
        },
        [&byte_code](sanema::LiteralDouble a_double) {
          byte_code.write(OPCODE::OP_PUSH_DOUBLE_CONST);
          byte_code.write(a_double.value);
        },
        [](sanema::LiteralString &string) {

        }
       );
}

void
generate_local_variable_access(ByteCode &byte_code, sanema::ByteCodeCompiler::Scope &context_frame_aux,
                               std::string identifier,
                               sanema::ByteCodeCompiler::GeneratorsMap &generator_map) {


  auto local_variable_entry = context_frame_aux.local_variables.at(identifier);
  auto opcode = match(local_variable_entry.declaration.type_identifier,
                      [](sanema::Integer &integer) {
                        switch (integer.size) {
                          case 8:
                            return OPCODE::OP_PUSH_LOCAL_SINT8;
                          case 16:
                            return OPCODE::OP_PUSH_LOCAL_SINT16;
                          case 32:
                            return OPCODE::OP_PUSH_LOCAL_SINT32;
                          case 64:
                            return OPCODE::OP_PUSH_LOCAL_SINT64;
                          default:
                            return OPCODE::OP_PUSH_LOCAL_SINT64;
                        }
                      },
                      [](sanema::Float &integer) {
                        return OPCODE::OP_PUSH_LOCAL_FLOAT;
                      },
                      [](sanema::Double &integer) {
                        return OPCODE::OP_PUSH_LOCAL_DOUBLE;
                      },
                      [](auto &ignore) {
                        return OPCODE::OP_PUSH_LOCAL_SINT64;
                      });
  byte_code.write(opcode);
  byte_code.write(local_variable_entry.address);
}

void
generate_function(ByteCode &byte_code, sanema::FunctionCall &function_call,
                  std::optional<sanema::DefineFunction> &function_definition,
                  sanema::ByteCodeCompiler::Scope &context_frame_aux,
                  sanema::ByteCodeCompiler::GeneratorsMap &generator_map) {
  if (generator_map.map.count(function_call.identifier) > 0) {
    generator_map.map.at(function_call.identifier)(byte_code,
                                                   function_call,
                                                   function_definition,
                                                   context_frame_aux,
                                                   generator_map);

  }
}

void
generate_add(ByteCode &byte_code, sanema::FunctionCall &function_call,
             std::optional<sanema::DefineFunction> const&function_definition,
             sanema::ByteCodeCompiler::Scope &context_frame_aux,
             sanema::ByteCodeCompiler::GeneratorsMap &generator_map) {
  int i = 0;
  if (function_call.arguments.size() != 2) {
    throw std::runtime_error("function add requires two arguments");
  }
  auto expression1 = function_call.arguments[0].expression;
  auto expression2 = function_call.arguments[1].expression;
  auto type1 = get_expression_type(function_call.arguments[0].expression,
                                   context_frame_aux);
  auto type2 = get_expression_type(function_call.arguments[1].expression,
                                   context_frame_aux);
  auto is_1_literal = std::holds_alternative<sanema::Literal>(expression1);
  auto is_2_literal = std::holds_alternative<sanema::Literal>(expression2);
  bool are_both_literals = is_2_literal && is_1_literal;
  auto return_type = function_definition.value().type;
  auto generate_expression_code = [&](sanema::Expression &expression, sanema::CompleteType &expression_type) {
    match(expression,
          [&](sanema::Literal literal) {

            generate_push_literal(byte_code,
                                  literal,
                                  context_frame_aux,
                                  generator_map,
                                  expression_type);
          },
          [&](sanema::FunctionCall function_call_nested) {
            auto function_definition_nested = get_function_definition(function_call,
                                                                      context_frame_aux);
            if (!function_definition_nested)
              throw std::runtime_error(std::format("function {} not found",
                                                   function_call_nested.identifier));
            generate_function(byte_code,
                              function_call_nested,
                              function_definition_nested,
                              context_frame_aux,
                              generator_map);
          },
          [&context_frame_aux, &byte_code, &generator_map](sanema::VariableEvaluation variable_evaluation) {
            generate_local_variable_access(byte_code,
                                           context_frame_aux,
                                           variable_evaluation.identifier,
                                           generator_map);
          });
  };


  generate_expression_code(expression1,
                           type1.value());

  generate_expression_code(expression2,
                           type2.value());
  OPCODE opcode = match(function_definition.value().type,
                        [](sanema::Integer const&integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_ADD_SINT8;
                            case 16:
                              return OPCODE::OP_ADD_SINT16;
                            case 32:
                              return OPCODE::OP_ADD_SINT32;
                            case 64:
                              return OPCODE::OP_ADD_SINT64;
                            default:
                              return OPCODE::OP_ADD_SINT64;
                          }
                        },
                        [](sanema::Float const&integer) {
                          return OPCODE::OP_ADD_FLOAT;
                        },
                        [](sanema::Double const&integer) {
                          return OPCODE::OP_ADD_DOUBLE;
                        },
                        [](auto const&ignore) {
                          return OPCODE::OP_ADD_SINT64;
                        });
  byte_code.write(opcode);

}


void
generate_set(ByteCode &byte_code, sanema::FunctionCall &function_call,
             std::optional<sanema::DefineFunction> const&function_definition,
             sanema::ByteCodeCompiler::Scope &context_frame_aux,
             sanema::ByteCodeCompiler::GeneratorsMap &generator_map) {
  uint64_t variable_address = 0;
  auto expression1 = function_call.arguments[0].expression;
  auto expression2 = function_call.arguments[1].expression;
  auto type1 = get_expression_type(function_call.arguments[0].expression,
                                   context_frame_aux);
  auto generate_push = [&](sanema::Expression &expression, sanema::CompleteType const &type) {
    match(expression,
          [&byte_code, &context_frame_aux, &generator_map, &type](sanema::Literal literal) {
            generate_push_literal(byte_code,
                                  literal,
                                  context_frame_aux,
                                  generator_map,
                                  type);
          },
          [&](sanema::FunctionCall function_call_nested) {
            auto function_definition_nested = get_function_definition(function_call_nested,
                                                                      context_frame_aux);
            if (!function_definition_nested)
              throw std::runtime_error(std::format("function {} not found",
                                                   function_call_nested.identifier));
            generate_function(byte_code,
                              function_call_nested,
                              function_definition_nested,
                              context_frame_aux,
                              generator_map);
          },
          [&context_frame_aux, &byte_code, &variable_address](sanema::VariableEvaluation variable_evaluation) {
            variable_address = context_frame_aux.local_variables.at(variable_evaluation.identifier).address;
          });
  };
  generate_push(expression1,type1.value());
  generate_push(expression2,type1.value());
  match(type1.value(),
        [&byte_code](sanema::Integer const &integer) {
          switch (integer.size) {
            case 8: byte_code.write(OPCODE::OP_SET_LOCAL_SINT8);break;
            case 16: byte_code.write(OPCODE::OP_SET_LOCAL_SINT16);break;
            case 32: byte_code.write(OPCODE::OP_SET_LOCAL_SINT32);break;
            case 64: byte_code.write(OPCODE::OP_SET_LOCAL_SINT64);break;
          };

        },
        [&byte_code](sanema::Float const &a_float) {
          byte_code.write(OPCODE::OP_SET_LOCAL_FLOAT);
        },
        [&byte_code](sanema::String const &integer) {

        },
        [&byte_code](sanema::Void const &a_void) {
          throw std::runtime_error("Void can't be set we should never reach this");
        },
        [&byte_code](sanema::Double const &a_double) {
          byte_code.write(OPCODE::OP_SET_LOCAL_DOUBLE);
        },
        [&byte_code](sanema::Boolean const &integer) {
//          byte_code.write(OPCODE::OP_SET_LOCAL_BOOL);
        },
        [&byte_code](sanema::Struct const &integer) {
//          byte_code.write(OPCODE::OP_SET_LOCAL_BOOL);
        }
       );
  byte_code.write(variable_address);
}

void sanema::ByteCodeCompiler::process(sanema::BlockOfCode &block_of_code) {
  std::vector<BlockOfCode> blocks_stack;
  scope_stack.emplace_back();
  function_bytecode_generators.map["add"] = generate_add;
  function_bytecode_generators.map["set"] = generate_set;
  add_built_in_functions(scope_stack.back().function_collection);
  for (auto &instruction: block_of_code.instructions) {
    match(instruction.instruction_sum,
          [this](DefineStruct &define_struct) {
            auto &scope = scope_stack.back();
            auto identifier = define_struct.user_type.value().type_id.identifier;
            if (scope.types.count(identifier) == 0) {
              scope.types[identifier] = define_struct;
            }
          },
          [this](DeclareVariable &declare_variable) {
            auto &current_scope = scope_stack.back();
            if (current_scope.local_variables.count(declare_variable.identifier) != 0) {
              throw std::runtime_error("variable " + declare_variable.identifier + " already defined");
            }
            uint64_t address = current_scope.context_address;
            current_scope.local_variables.emplace(declare_variable.identifier,
                                                  VariableEntry{declare_variable, address});

            auto size = get_type_size(declare_variable.type_identifier);
            current_scope.context_address += size;
            byte_code.write(OPCODE::OP_RESERVE_STACK_SPACE);
            byte_code.write(size);
            FunctionCall function_call;
            function_call.identifier="set";
            function_call.arguments.emplace_back(VariableEvaluation{declare_variable.identifier});
            function_call.arguments.emplace_back(get_default_literal_for_type(declare_variable.type_identifier));
            generate_set(byte_code,function_call,{},current_scope,function_bytecode_generators);


          },
          [this](DefineFunction &function) {
            auto define_function_copy = function;
            define_function_copy.body = BlockOfCode{};
            auto function_entry = find_function(scope_stack.back().function_collection,
                                                define_function_copy);
            if (!function_entry) {
              if (scope_stack.back().function_collection.count(define_function_copy.identifier) == 0) {
                scope_stack.back().function_collection[define_function_copy.identifier] = {};
              }
              scope_stack.back().function_collection[define_function_copy.identifier].overloads.emplace_back(define_function_copy);
            } else {
              throw (std::runtime_error("function already defined"));
            }
          },
          [this](FunctionCall &function_call) {
            auto function_definition = get_function_definition(function_call,
                                                               scope_stack.back());

            if (!function_definition.has_value()) {
              std::string message="function " + function_call.identifier +"( " ;
              std::string separator{""};
               for(auto& parameter:function_call.arguments){
                 message+=separator+" "+ type_to_string(get_expression_type(parameter.expression,scope_stack.back()).value()) ;
                 separator=",";
               }
               message+=") not found";
              throw std::runtime_error(message);
            }
            generate_function(byte_code,
                              function_call,
                              function_definition,
                              scope_stack.back(),
                              function_bytecode_generators);
          },
          [](BlockOfCode &block_of_code) {

          }
         );
  }
}
