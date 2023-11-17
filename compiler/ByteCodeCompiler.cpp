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
get_variable_type(sanema::VariableEvaluation &variable, sanema::ByteCodeCompiler::Scope &scope) {
  auto context = scope;
  if (context.local_variables.count(variable.identifier) > 0) {
    return context.local_variables.at(variable.identifier).declaration.type_identifier;
  }
  return {};
}

std::optional<sanema::CompleteType>
get_expression_type(sanema::Expression &expression, sanema::ByteCodeCompiler::Scope &scope) {
  return match(expression,
               [&](sanema::FunctionCall &function_call_nested) -> std::optional<sanema::CompleteType> {
                 auto function_definition_nested = get_function_definition(function_call_nested,
                                                                           scope);
                 if (function_definition_nested.has_value()) {
                   return function_definition_nested.value().type;
                 } else {
                   std::string message = std::format("function {}  (",
                                                     function_call_nested.identifier);
                   std::string separator = "";
                   for (auto &parameters: function_definition_nested->parameters) {
                     message += std::format("{} {}",
                                            separator,
                                            sanema::type_to_string(parameters.type.value()));
                     separator = ",";
                   }
                   message += ") not found";
                   throw std::runtime_error(message);
                   return {};
                 }
               },
               [&](sanema::VariableEvaluation &variable) -> std::optional<sanema::CompleteType> {
                 return get_variable_type(variable,
                                          scope);
               },
               [&](sanema::Literal &literal) -> std::optional<sanema::CompleteType> {
                 return sanema::get_literal_type(literal);
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
    function_definition.parameters.emplace_back(sanema::FunctionParameter{"", {}, type.value()});
  }
  auto found_function = scope.function_collection.find_function(function_definition);
  return found_function;
}

void generate_if(sanema::ByteCode &byte_code, sanema::FunctionCall &function_call,
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
generate_push_literal(sanema::ByteCode &byte_code,
                      sanema::Literal &literal,
                      sanema::ByteCodeCompiler::Scope &context_frame_aux,
                      sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
                      sanema::CompleteType const &type) {
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
        [&byte_code](sanema::LiteralString &string) {
          byte_code.write(OPCODE::OP_PUSH_STRING_CONST);
          auto index = byte_code.add_string_literal(string.value);
          byte_code.write(sanema::StringReference{sanema::StringLocation::LiteralPool, (std::uint32_t) index});
        }
       );
}

void
generate_local_variable_access(sanema::ByteCode &byte_code, sanema::ByteCodeCompiler::Scope &context_frame_aux,
                               std::string identifier,
                               bool copy) {

    auto local_variable_entry = context_frame_aux.local_variables.at(identifier);
    if(copy) {
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
    }else {
      byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
      byte_code.write(local_variable_entry.address);
    }
}

std::optional<sanema::DefineFunction>
generate_function_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map) {

  sanema::DefineFunction function_definition;
  function_definition.identifier = function_call.identifier;
  auto final_function_definition = get_function_definition(function_call,
                                                           context_frame_aux);
  if (!final_function_definition.has_value())
    throw std::runtime_error("can't generate function " + function_call.identifier);
  for (int i = 0; i < final_function_definition.value().parameters.size(); i++) {
    auto &argument = function_call.arguments[i];
    auto &parameter = final_function_definition.value().parameters[i];
    sanema::CompleteType type = match(argument.expression,
                                      [&](sanema::Literal literal) -> sanema::CompleteType {
                                        auto literal_type = sanema::get_literal_type(literal);
                                        if (parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE) {
                                          throw std::runtime_error("can't bind literal to a mutable reference");
                                        }
                                        generate_push_literal(byte_code,
                                                              literal,
                                                              context_frame_aux,
                                                              generator_map,
                                                              literal_type);
                                        byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
                                        byte_code.write(static_cast<std::uint64_t>(sanema::FunctionParameterType::Value));
                                        return literal_type;
                                      },
                                      [&](sanema::FunctionCall function_call_nested) -> sanema::CompleteType {
                                        auto definition = generate_function_call(byte_code,
                                                                                 function_call_nested,
                                                                                 context_frame_aux,
                                                                                 generator_map);
                                        if (!definition) {
                                          //We should not reach this
                                          throw std::runtime_error(std::format(
                                            "function {} not found, but that is unexpected",
                                            function_call_nested.identifier));
                                        }
                                        if (parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE) {
                                          throw std::runtime_error("can't bind temporary value  to a mutable reference");
                                        }
                                        byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
                                        byte_code.write(static_cast<std::uint64_t>(sanema::FunctionParameterType::Value));
                                        return definition->type;
                                      },
                                      [&](sanema::VariableEvaluation variable_evaluation) -> sanema::CompleteType {
                                        auto variable_type = get_variable_type(variable_evaluation,
                                                                               context_frame_aux);
                                        if (!variable_type.has_value()) {
                                          throw std::runtime_error(std::format("variable  {} not found ",
                                                                               variable_evaluation.identifier));
                                        }
                                        bool should_copy=false;
                                        switch (parameter.modifier) {
                                          case  sanema::FunctionParameter::Modifier::MUTABLE:
                                            should_copy=false;
                                            break;
                                          case sanema::FunctionParameter::Modifier::CONST:
                                            should_copy=false;
                                            break;
                                          case sanema::FunctionParameter::Modifier::VALUE:
                                            should_copy=true;
                                            break;
                                        }
                                        generate_local_variable_access(byte_code,
                                                                       context_frame_aux,
                                                                       variable_evaluation.identifier,
                                                                       should_copy);
                                        byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
                                        if(should_copy){
                                          byte_code.write(static_cast<std::uint64_t>(sanema::FunctionParameterType::Value));
                                        }else {
                                          byte_code.write(static_cast<std::uint64_t>(sanema::FunctionParameterType::VariableReferece));
                                        }
                                        return variable_type.value();
                                      });
    //TODO i am not sure if we need to  specify the correct Modifier, need to check later
    function_definition.parameters.emplace_back("",
                                                sanema::FunctionParameter::Modifier::VALUE,
                                                type);
  }
  if (!final_function_definition.has_value()) {
    std::string message = std::format("function {}  (",
                                      function_call.identifier);
    std::string separator = "";
    for (auto &parameters: function_definition.parameters) {
      message += std::format("{} {}",
                             separator,
                             sanema::type_to_string(parameters.type.value()));
      separator = ",";
    }
    message += ") not found";
    throw std::runtime_error(message);
  }
  if (generator_map.map.count(function_call.identifier) > 0) {
    generator_map.map.at(function_call.identifier)(byte_code,
                                                   final_function_definition);
  } else if (final_function_definition->external_id) {
    byte_code.write(OPCODE::OP_CALL_EXTERNAL_FUNCTION);
    byte_code.write(final_function_definition->external_id.value());
  } else {
    byte_code.write(OPCODE::OP_CALL);
    byte_code.write(final_function_definition->id);
  }
  return final_function_definition;
}

void

generate_add(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition) {
  OPCODE opcode = match(function_definition.value().type,
                        [](sanema::Integer const &integer) {
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
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_ADD_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_ADD_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_ADD_SINT64;
                        });
  byte_code.write(opcode);
}


void
generate_set(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition) {
  match(function_definition->type,
        [&byte_code](sanema::Integer const &integer) {
          switch (integer.size) {
            case 8:
              byte_code.write(OPCODE::OP_SET_LOCAL_SINT8);
              break;
            case 16:
              byte_code.write(OPCODE::OP_SET_LOCAL_SINT16);
              break;
            case 32:
              byte_code.write(OPCODE::OP_SET_LOCAL_SINT32);
              break;
            case 64:
              byte_code.write(OPCODE::OP_SET_LOCAL_SINT64);
              break;
          };
        },
        [&byte_code](sanema::Float const &a_float) {
          byte_code.write(OPCODE::OP_SET_LOCAL_FLOAT);
        },
        [&byte_code](sanema::String const &integer) {
          byte_code.write(OPCODE::OP_SET_LOCAL_STRING);
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
}

void sanema::ByteCodeCompiler::process(sanema::BlockOfCode &block_of_code, FunctionCollection &built_in_functions) {
  std::vector<BlockOfCode> blocks_stack;
  scope_stack.emplace_back();
  function_bytecode_generators.map["add"] = generate_add;
  function_bytecode_generators.map["set"] = generate_set;
  scope_stack.back().function_collection = built_in_functions;

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
            function_call.identifier = "set";
            function_call.arguments.emplace_back(VariableEvaluation{declare_variable.identifier});
            function_call.arguments.emplace_back(get_default_literal_for_type(declare_variable.type_identifier));
            auto function_definition = get_function_definition(function_call,
                                                               current_scope);
            generate_function_call(byte_code,
                                   function_call,
                                   current_scope,
                                   function_bytecode_generators);
          },
          [this](DefineFunction &function) {
            auto function_entry = scope_stack.back().function_collection.find_function(function);
            if (!function_entry) {
              scope_stack.back().function_collection.add_function(function);
            } else {
              throw (std::runtime_error("function already defined"));
            }
          },
          [this](FunctionCall &function_call) {
            generate_function_call(byte_code,
                                   function_call,
                                   scope_stack.back(),
                                   function_bytecode_generators);
          },
          [](BlockOfCode &block_of_code) {
          }
         );
  }
}
