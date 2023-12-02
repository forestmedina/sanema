//
// Created by fores on 11/3/2023.
//

#include "ByteCodeCompiler.h"

#include <format>
#include <vector>
#include <built-in/built_in_functions.h>
#include <util/lambda_visitor.hpp>
#include "../vm/opcodes.h"
#include <boost/algorithm/string.hpp>
#include <built-in/generators.h>

bool is_field_identifier(std::string_view identifier) {
  auto pos = identifier.find('.');
  bool is_field = pos != std::string::npos;
  return is_field;
}

std::tuple<std::string, std::string> split_identifier(std::string identifier_p) {
  auto pos = identifier_p.find('.');
  bool is_field = pos != std::string::npos;
  std::string identifier;
  std::string field_identifier;
  if (is_field) {
    identifier = identifier_p.substr(0,
                                     pos);
    field_identifier = identifier_p.substr(pos + 1,
                                           identifier_p.size() - pos - 1);
  }
  else {
    identifier = identifier_p;
  }
  return {identifier, field_identifier};
}

std::optional<sanema::DefineFunction> get_function_definition(sanema::FunctionCall&function_call,
                                                              sanema::ByteCodeCompiler::Scope&scope);


std::optional<sanema::CompleteType>
get_variable_type(sanema::VariableEvaluation&variable, sanema::ByteCodeCompiler::Scope&scope) {
  bool is_field = is_field_identifier(variable.identifier);
  auto [identifier, field_name] = split_identifier(variable.identifier);
  if (scope.local_variables.count(std::string(identifier)) > 0) {
    if (is_field) {
      auto type = match(scope.local_variables.at(std::string(identifier)).declaration,
                        [](sanema::DeclareVariable&variable_value) {
                          return variable_value.type_identifier;
                        },
                        [](sanema::FunctionParameter&parameter) {
                          return parameter.type.value();
                        }
      );
      return match(type,
                   [field_name, &scope](sanema::UserDefined&user_defined) -> sanema::CompleteType {
                     auto final_type = scope.types.find_type(user_defined);
                     if (!final_type.has_value()) {
                       throw std::runtime_error(std::format("user defined type {} do not exists  ",
                                                            user_defined.type_id.identifier));
                     }
                     auto field = final_type.value().get_field(field_name);
                     if (!field) {
                       throw std::runtime_error(std::format("field {} do not exists for user defined type {} ",
                                                            field_name,
                                                            user_defined.type_id.identifier));
                     }
                     return field->type.value();
                   },
                   [field_name](auto&primitive) -> sanema::CompleteType {
                     throw std::runtime_error(std::format("type {} have no field {} ",
                                                          sanema::type_to_string(primitive),
                                                          field_name));
                   }
      );
    }
    else {
      return match(scope.local_variables.at(std::string(identifier)).declaration,
                   [](sanema::DeclareVariable&variable_value) {
                     return variable_value.type_identifier;
                   },
                   [](sanema::FunctionParameter&parameter) {
                     return parameter.type.value();
                   }
      );
    }
  }
  return {};
}

std::optional<sanema::CompleteType>
get_expression_type(sanema::Expression&expression, sanema::ByteCodeCompiler::Scope&scope) {
  return match(expression,
               [&](sanema::FunctionCall&function_call_nested) -> std::optional<sanema::CompleteType> {
                 auto function_definition_nested = get_function_definition(function_call_nested,
                                                                           scope);
                 if (function_definition_nested.has_value()) {
                   return function_definition_nested.value().type;
                 }
                 else {
                   std::string message = std::format("function {}  (",
                                                     function_call_nested.identifier);
                   std::string separator;
                   for (auto&parameters: function_definition_nested->parameters) {
                     message += std::format("{} {}",
                                            separator,
                                            sanema::type_to_string(parameters.type.value()));
                     separator = ",";
                   }
                   message += ") not found";
                   throw std::runtime_error(message);
                 }
               },
               [&](sanema::VariableEvaluation&variable) -> std::optional<sanema::CompleteType> {
                 return get_variable_type(variable,
                                          scope);
               },
               [&](sanema::Literal&literal) -> std::optional<sanema::CompleteType> {
                 return sanema::get_literal_type(literal);
               }
  );
}

std::optional<sanema::DefineFunction> get_function_definition(sanema::FunctionCall&function_call,
                                                              sanema::ByteCodeCompiler::Scope&scope) {
  sanema::DefineFunction function_definition;
  function_definition.identifier = function_call.identifier;
  for (auto&argument: function_call.arguments) {
    auto type = get_expression_type(argument.expression,
                                    scope);
    if (!type.has_value()) return {};
    function_definition.parameters.emplace_back(sanema::FunctionParameter{"", {}, type.value()});
  }
  auto found_function = scope.function_collection.find_function(function_definition);
  if (!found_function) {
    std::string message = std::format("function {}  (",
                                      function_definition.identifier);
    std::string separator;
    for (auto&parameters: function_definition.parameters) {
      message += std::format("{} {}",
                             separator,
                             sanema::type_to_string(parameters.type.value()));
      separator = ",";
    }
    message += ") not found";
    throw std::runtime_error(message);
  }

  return found_function;
}

void generate_if(sanema::ByteCode&byte_code, sanema::FunctionCall&function_call,
                 sanema::ByteCodeCompiler::Scope&context_frame_aux,
                 sanema::ByteCodeCompiler::GeneratorsMap&generator_map) {
  byte_code.write(OPCODE::OP_JUMP_IF_FALSE);
}

template<class T>
T convert_number_literal(sanema::Literal literal) {
  return match(literal,
               [&](sanema::LiteralString&string) {
                 return T{};
               },
               [&](auto&literal_value) {
                 return static_cast<T>(literal_value.value);
               }
  );
}

void generate_push_temp_variable(sanema::ByteCode&byte_code,
                                 sanema::Literal&literal,
                                 sanema::ByteCodeCompiler::Scope&scope,
                                 sanema::ByteCodeCompiler::GeneratorsMap&generator_map,
                                 sanema::CompleteType const&type) {
  //TODO check if the wy we handle temp variables did changed when we consolidated the stack and operandstack
  auto address = scope.scope_address;
  scope.reserve_space_for_type(type);
  match(literal,
        [&byte_code, &address](sanema::LiteralSInt64 int_64) {
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
          byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
          byte_code.write(int_64.value);
          byte_code.write(OPCODE::OP_SET_LOCAL_SINT64);
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        },
        [&byte_code, &address](sanema::LiteralSInt32 int_32) {
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
          byte_code.write(OPCODE::OP_PUSH_SINT32_CONST);
          byte_code.write(int_32.value);
          byte_code.write(OPCODE::OP_SET_LOCAL_SINT32);
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        },
        [&byte_code, &address](sanema::LiteralSInt16 int_16) {
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
          byte_code.write(OPCODE::OP_PUSH_SINT16_CONST);
          byte_code.write(int_16.value);

          byte_code.write(OPCODE::OP_SET_LOCAL_SINT16);
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        },
        [&byte_code, &address](sanema::LiteralSInt8 int_8) {
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
          byte_code.write(OPCODE::OP_PUSH_SINT8_CONST);
          byte_code.write(int_8.value);

          byte_code.write(OPCODE::OP_SET_LOCAL_SINT8);
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        },
        [&byte_code, &address](sanema::LiteralBoolean boolean) {
          //TODO Boolean are not implemented
          byte_code.write(boolean.value ? OPCODE::OP_TRUE : OPCODE::OP_FALSE);
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        },
        [&byte_code, &address](sanema::LiteralFloat a_float) {
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
          byte_code.write(OPCODE::OP_PUSH_FLOAT_CONST);
          byte_code.write(a_float.value);

          byte_code.write(OPCODE::OP_SET_LOCAL_FLOAT);
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        },
        [&byte_code, &address](sanema::LiteralDouble a_double) {
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
          byte_code.write(OPCODE::OP_PUSH_DOUBLE_CONST);
          byte_code.write(a_double.value);
          byte_code.write(OPCODE::OP_SET_LOCAL_DOUBLE);
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        },
        [&byte_code, &address, type](sanema::LiteralString&string) {
          byte_code.write(OPCODE::OP_RESERVE_STACK_SPACE);
          byte_code.write(sanema::get_type_size(type));
          //** this address will be used by the set
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);

          byte_code.write(OPCODE::OP_PUSH_STRING_CONST);
          auto index = byte_code.add_string_literal(string.value);
          byte_code.write(sanema::StringReference{sanema::StringLocation::LiteralPool, (std::uint32_t)index});

          byte_code.write(OPCODE::OP_SET_LOCAL_STRING);
          //** this address will be used by the function to know where the parameter is
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        }
  );
}

void
generate_push_const_literal(sanema::ByteCode&byte_code,
                            sanema::Literal&literal,
                            sanema::ByteCodeCompiler::Scope&context_frame_aux,
                            sanema::ByteCodeCompiler::GeneratorsMap&generator_map,
                            sanema::CompleteType const&type) {
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
        [&byte_code](sanema::LiteralString&string) {
          byte_code.write(OPCODE::OP_PUSH_STRING_CONST);
          auto index = byte_code.add_string_literal(string.value);
          byte_code.write(sanema::StringReference{sanema::StringLocation::LiteralPool, (std::uint32_t)index});
        }
  );
}

void
generate_local_variable_access(sanema::ByteCode&byte_code, sanema::ByteCodeCompiler::Scope&context_frame_aux,
                               std::string identifier_p,
                               bool copy) {
  auto pos = identifier_p.find('.');
  bool is_field = is_field_identifier(identifier_p);

  std::vector<std::string> accesors_list;
  boost::split(accesors_list,
               identifier_p,
               boost::is_any_of("."));
  auto [identifier, field_identifier] = split_identifier(identifier_p);
  std::string temp_identifier = std::string(identifier);
  auto local_variable_entry = context_frame_aux.local_variables.at(temp_identifier);
  std::int64_t address = local_variable_entry.address;
  auto type = match(local_variable_entry.declaration,
                    [](sanema::DeclareVariable&variable) {
                      return variable.type_identifier;
                    },
                    [](sanema::FunctionParameter&parameter) {
                      return parameter.type.value();
                    }
  );
  bool is_external = false;

  if (is_field) {
    match_base<sanema::UserDefined &>(
      type,
      [&field_identifier, &byte_code, &is_external, &address, &type, &context_frame_aux](
      sanema::UserDefined&user_defined) {
        auto final_type = context_frame_aux.types.find_type(user_defined);
        is_external = final_type.value().external_id.has_value();
        auto field = final_type.value().get_field(field_identifier);
        if (is_external) {
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
          byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
          byte_code.write(final_type->external_id.value());
          byte_code.write(OPCODE::OP_PUSH_SINT64_CONST);
          byte_code.write(field->offset);
          byte_code.write(OPCODE::OP_PUSH_EXTERNAL_FIELD_ADDRESS);
        }
        else {
          if (field != nullptr) {
            type = field->type.value();
            address += boost::numeric_cast<std::int64_t>(field->offset);
          }
          byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
          byte_code.write(address);
        }
      });
  }
  else {
    byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
    byte_code.write(address);
  }
  bool is_reference = match(local_variable_entry.declaration,
                            [](sanema::DeclareVariable&variable) {
                              return false;
                            },
                            [](sanema::FunctionParameter&parameter) {
                              return parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE ||
                                     parameter.modifier == sanema::FunctionParameter::Modifier::CONST;
                            }
  );

  if (is_reference) {
    //TODO we are pushing the address as SINT64 whe may use a new Operand called PUSH ADDRESS
    byte_code.write(OPCODE::OP_PUSH_LOCAL_SINT64);
  }
  if (copy) {
    auto opcode = match(type,
                        [](sanema::Integer&integer) {
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
                        [](sanema::Float&integer) {
                          return OPCODE::OP_PUSH_LOCAL_FLOAT;
                        },
                        [](sanema::Double&integer) {
                          return OPCODE::OP_PUSH_LOCAL_DOUBLE;
                        },
                        [](auto&ignore) {
                          return OPCODE::OP_PUSH_LOCAL_SINT64;
                        });

    byte_code.write(opcode);
  }
}


void
generate_set(sanema::ByteCode&byte_code, std::optional<sanema::DefineFunction> const&function_definition) {
  match(function_definition->type,
        [&byte_code](sanema::Integer const&integer) {
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
        [&byte_code](sanema::Float const&a_float) {
          byte_code.write(OPCODE::OP_SET_LOCAL_FLOAT);
        },
        [&byte_code](sanema::String const&integer) {
          byte_code.write(OPCODE::OP_SET_LOCAL_STRING);
        },
        [](sanema::Void const&a_void) {
          throw std::runtime_error("Void can't be set we should never reach this");
        },
        [&byte_code](sanema::Double const&a_double) {
          byte_code.write(OPCODE::OP_SET_LOCAL_DOUBLE);
        },
        [&byte_code](sanema::Boolean const&integer) {
          //          byte_code.write(OPCODE::OP_SET_LOCAL_BOOL);
        },
        [&byte_code](sanema::UserDefined const&integer) {
          //          byte_code.write(OPCODE::OP_SET_LOCAL_BOOL);
        }
  );
}

void
generate_return(sanema::ByteCode&byte_code, std::optional<sanema::DefineFunction> const&function_definition) {
  byte_code.write(OPCODE::OP_RETURN);
}

std::optional<sanema::DefineFunction>
generate_function_call(
  sanema::ByteCode&byte_code,
  sanema::FunctionCall&function_call,
  sanema::ByteCodeCompiler::Scope&context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap&generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution>&function_call_sustitutions
);

void generate_expression_access(
  sanema::Expression const&expression,
  sanema::FunctionParameter::Modifier modifier,
  sanema::CompleteType const&type,
  sanema::ByteCode&byte_code,
  sanema::ByteCodeCompiler::Scope&context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap&generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution>&function_call_sustitutions
) {
  match(expression,
        [&](sanema::Literal literal) -> void {
          auto literal_type = sanema::get_literal_type(literal);
          if (modifier == sanema::FunctionParameter::Modifier::MUTABLE) {
            throw std::runtime_error("can't bind literal to a mutable reference");
          }
          if (modifier == sanema::FunctionParameter::Modifier::CONST) {
            generate_push_temp_variable(byte_code,
                                        literal,
                                        context_frame_aux,
                                        generator_map,
                                        literal_type);
          }
          else {
            generate_push_const_literal(byte_code,
                                        literal,
                                        context_frame_aux,
                                        generator_map,
                                        literal_type);
          }
        },
        [&](sanema::FunctionCall function_call_nested) -> void {
          if (modifier == sanema::FunctionParameter::Modifier::MUTABLE) {
            throw std::runtime_error("can't bind temporary value  to a mutable reference");
          }
          bool is_reference = modifier == sanema::FunctionParameter::Modifier::CONST;
          auto address = context_frame_aux.scope_address;
          if (is_reference) {
            context_frame_aux.reserve_space_for_type(type);
            byte_code.write(OPCODE::OP_RESERVE_STACK_SPACE);
            byte_code.write(sanema::get_type_size(type));
            byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
            byte_code.write(address);
          }
          auto definition = generate_function_call(byte_code,
                                                   function_call_nested,
                                                   context_frame_aux,
                                                   generator_map,
                                                   function_call_sustitutions);
          if (is_reference) {
            generate_set(byte_code,
                         sanema::DefineFunction{"", type});
            byte_code.write(OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL);
            byte_code.write(address);
          }
          if (!definition) {
            //We should not reach this
            throw std::runtime_error(std::format(
              "function {} not found, but that is unexpected",
              function_call_nested.identifier));
          }
        },
        [&](sanema::VariableEvaluation variable_evaluation) -> void {
          auto variable_type = get_variable_type(variable_evaluation,
                                                 context_frame_aux);
          if (!variable_type.has_value()) {
            throw std::runtime_error(std::format("variable  {} not found ",
                                                 variable_evaluation.identifier));
          }
          bool should_copy = false;
          switch (modifier) {
            case sanema::FunctionParameter::Modifier::MUTABLE:
              should_copy = false;
              break;
            case sanema::FunctionParameter::Modifier::CONST:
              should_copy = false;
              break;
            case sanema::FunctionParameter::Modifier::VALUE:
              should_copy = true;
              break;
          }
          generate_local_variable_access(byte_code,
                                         context_frame_aux,
                                         variable_evaluation.identifier,
                                         should_copy);
        });
}

std::optional<sanema::DefineFunction>
generate_function_call(
  sanema::ByteCode&byte_code,
  sanema::FunctionCall&function_call,
  sanema::ByteCodeCompiler::Scope&context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap&generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution>&function_call_sustitutions
) {
  sanema::DefineFunction function_definition;
  function_definition.identifier = function_call.identifier;
  auto final_function_definition = get_function_definition(function_call,
                                                           context_frame_aux);
  if (!final_function_definition.has_value()) {
    throw std::runtime_error("can't generate function " + function_call.identifier);
  }
  for (int i = 0; i < final_function_definition.value().parameters.size(); i++) {
    auto&argument = function_call.arguments[i];
    auto&parameter = final_function_definition.value().parameters[i];
    generate_expression_access(argument.expression,
                               parameter.modifier,
                               parameter.type.value(),
                               byte_code,
                               context_frame_aux,
                               generator_map,
                               function_call_sustitutions);
  }

  if (generator_map.map.count(function_call.identifier) > 0) {
    generator_map.map.at(function_call.identifier)(byte_code,
                                                   final_function_definition);
  }
  else if (final_function_definition->external_id) {
    byte_code.write(OPCODE::OP_CALL_EXTERNAL_FUNCTION);
    byte_code.write(final_function_definition->external_id.value());
  }
  else {
    byte_code.write(OPCODE::OP_CALL);
    auto address = byte_code.get_current_address();
    byte_code.write(static_cast<std::uint64_t>(final_function_definition->id));
    byte_code.write<std::uint32_t>(0);
    auto sustitution_iter = std::find_if(function_call_sustitutions.begin(),
                                         function_call_sustitutions.end(),
                                         [&final_function_definition](
                                         sanema::ByteCodeCompiler::FuctionCallSustitution&sustitution) {
                                           return sustitution.function_id == final_function_definition->id;
                                         });
    if (sustitution_iter == function_call_sustitutions.end()) {
      function_call_sustitutions.emplace_back(std::vector{address},
                                              0,
                                              0,
                                              final_function_definition->id);
    }
    else {
      sustitution_iter->caller_addresses.emplace_back(address);
    }
  }
  return final_function_definition;
}


void sanema::ByteCodeCompiler::Scope::reserve_space_for_type(CompleteType const&type) {
  auto size = get_type_size(type);
  scope_address.address += size;
}


std::uint32_t
sanema::ByteCodeCompiler::generate_block(sanema::BlockOfCode&block_of_code, FunctionCollection&built_in_functions,
                                         TypeCollection&external_types) {
  std::int64_t total_variable_space = 0;

  for (auto&instruction: block_of_code.instructions) {
    match(instruction.instruction_sum,
          [this](DefineStruct&define_struct) {
            auto&scope = scope_stack.back();
            if (!define_struct.user_type.has_value()) {
              throw std::runtime_error("user type has no value ");
            }

            auto identifier = define_struct.user_type.value().type_id.identifier;
            if (!scope.types.containts(identifier)) {
              std::uint64_t offset = 0;
              for (auto&field: define_struct.user_type->fields) {
                field.offset = offset;
                offset += get_type_size(field.type.value());
              }
              scope.types.add_type(define_struct.user_type.value());
            }
          },
          [&](IfStatement&if_statement) {
            auto current_scope = scope_stack.back();
            generate_expression_access(if_statement.expression,
                                       FunctionParameter::Modifier::VALUE,
                                       CompleteType{sanema::Boolean{}},
                                       byte_code,
                                       current_scope,
                                       function_bytecode_generators,
                                       function_call_sustitutions);

            byte_code.write(OPCODE::OP_JUMP_IF_FALSE);
            std::uint64_t address_false_jump_offset = byte_code.get_current_address(); //Address of the jump instruction
            byte_code.write(std::uint64_t(0));
            std::uint64_t address_false_jump_instruction = byte_code.get_current_address();
            generate_block(if_statement.true_path,
                           built_in_functions,
                           external_types);

            byte_code.write(OPCODE::OP_JUMP);
            std::uint64_t address_true_jump_offset = byte_code.get_current_address();
            byte_code.write(std::uint64_t(0));
            std::uint64_t address_true_jump_instruction = byte_code.get_current_address();
            //Address of the jump instruction
            auto address_false_branch = byte_code.get_current_address();

            generate_block(if_statement.false_path,
                           built_in_functions,
                           external_types);
            auto address_end_if = byte_code.get_current_address();
            write_to_byte_code(byte_code.code_data,
                               address_true_jump_offset,
                               address_end_if - address_true_jump_instruction);
            write_to_byte_code(byte_code.code_data,
                               address_false_jump_offset,
                               address_false_branch - address_false_jump_instruction);
          },
          [this,&total_variable_space](DeclareVariable&declare_variable) {
            auto&current_scope = scope_stack.back();
            if (current_scope.local_variables.count(declare_variable.identifier) != 0) {
              throw std::runtime_error("variable " + declare_variable.identifier + " already defined");
            }

            current_scope.local_variables.emplace(declare_variable.identifier,
                                                  VariableEntry{declare_variable, total_variable_space});
            current_scope.reserve_space_for_type(declare_variable.type_identifier);


            if (is_user_defined(declare_variable.type_identifier)) {
              //TODO we need to call the constructor here but for now we will initialize each field with
              //their default value
              UserDefined user_defined = std::get<UserDefined>(declare_variable.type_identifier);
              auto final_type = current_scope.types.find_type(declare_variable.type_identifier);
              if (final_type.has_value()) {
                // byte_code.write(OPCODE::OP_RESERVE_STACK_SPACE);
                total_variable_space += get_type_size(final_type.value());
                for (auto&field: final_type.value().fields) {
                  FunctionCall function_call;
                  function_call.identifier = "set";
                  function_call.arguments.emplace_back(VariableEvaluation{
                    declare_variable.identifier + "." + field.identifier
                  });
                  function_call.arguments.emplace_back(get_default_literal_for_type(field.type.value()));
                  generate_function_call(byte_code,
                                         function_call,
                                         current_scope,
                                         function_bytecode_generators,
                                         function_call_sustitutions);
                }
              }
              else {
                throw std::runtime_error(std::format("Type {} does not exists",
                                                     user_defined.type_id.identifier));
              }
            }
            else {
              total_variable_space += get_type_size(declare_variable.type_identifier);
              FunctionCall function_call;
              function_call.identifier = "set";
              function_call.arguments.emplace_back(VariableEvaluation{declare_variable.identifier});
              function_call.arguments.emplace_back(get_default_literal_for_type(declare_variable.type_identifier));
              generate_function_call(byte_code,
                                     function_call,
                                     current_scope,
                                     function_bytecode_generators,
                                     function_call_sustitutions);
            }
          },
          [this](DefineFunction&function) {
            auto function_entry = scope_stack.back().function_collection.find_function(function);
            if (!function_entry) {
              auto id = scope_stack.back().function_collection.add_function(function);
              pendind_to_generate_functions.emplace_back(id);
            }
            else {
              throw std::runtime_error(std::format("function {} already defined",
                                                   function.identifier));
            }
          },
          [this](FunctionCall&function_call) {
            generate_function_call(byte_code,
                                   function_call,
                                   scope_stack.back(),
                                   function_bytecode_generators,
                                   function_call_sustitutions);
          },
          [&](BlockOfCode&block_of_code) {
            total_variable_space += generate_block(block_of_code,
                                                   built_in_functions,
                                                   external_types);
          }
    );
  }


  return total_variable_space;
}

void sanema::ByteCodeCompiler::process(sanema::BlockOfCode&block_of_code, FunctionCollection&built_in_functions,
                                       TypeCollection&external_types) {
  std::vector<BlockOfCode> blocks_stack;
  scope_stack.emplace_back();
  function_bytecode_generators.map["add"] = generate_add;
  function_bytecode_generators.map["subtract"] = generate_subtract;
  function_bytecode_generators.map["multiply"] = generate_multiply;
  function_bytecode_generators.map["divide"] = generate_divide;
  function_bytecode_generators.map["set"] = generate_set;
  function_bytecode_generators.map["greater"] = generate_greater;
  function_bytecode_generators.map["less"] = generate_less;
  function_bytecode_generators.map["equal"] = generate_equal;
  function_bytecode_generators.map["return"] = generate_return;
  scope_stack.back().function_collection = built_in_functions;
  scope_stack.back().types = external_types;
  std::optional<BlockOfCode> next_block = block_of_code;
  bool main = true;
  while (next_block) {
    byte_code.write(OPCODE::OP_RESERVE_STACK_SPACE);
    auto addres_variable_space = byte_code.write<std::uint64_t>(0);
    auto total_variable_space = generate_block(next_block.value(),
                                               built_in_functions,
                                               external_types);
    write_to_byte_code(byte_code.code_data,
                       OPCODE::OP_RETURN);
    byte_code.write_to_address<std::uint64_t>(total_variable_space,
                                              addres_variable_space);
    next_block = {};
    if (!pendind_to_generate_functions.empty()) {
      auto pending_function = pendind_to_generate_functions.back();
      auto&scope = scope_stack.back();
      auto function = scope.function_collection.get_function_by_id(pending_function);
      if (function == nullptr) {
        throw std::runtime_error("can't find function , this is unexpected, maybe a bug in the compiler");
      }

      // ReSharper disable once CppDFANullDereference
      next_block = function->body;
      auto scope_copy = scope;

      std::uint64_t function_address = byte_code.get_current_address();
      //      std::cout << "Determining function address : " << function_address << "\n";

      scope_copy.scope_address.address = 0;
      std::int64_t parameter_address = 0;
      scope_copy.local_variables.clear();
      std::int32_t parameters_size = 0;
      for (auto & parameter : function->parameters) {
        parameter_address = parameters_size;
        if (parameter.modifier == FunctionParameter::Modifier::CONST ||
            parameter.modifier == FunctionParameter::Modifier::MUTABLE) {
          parameters_size += boost::numeric_cast<std::int64_t>(get_type_size(Integer(64)));;
        }
        else {
          parameters_size += boost::numeric_cast<std::int64_t>(get_type_size(parameter.type.value()));
        }
        scope_copy.local_variables.emplace(parameter.identifier,
                                           VariableEntry{parameter, parameter_address});
      }
      auto found_sustition=std::find_if(function_call_sustitutions.begin(),function_call_sustitutions.end(),
                            [&pending_function, function_address,&parameters_size](FuctionCallSustitution& sustitution) {
                              return sustitution.function_id == pending_function;
                            });
      if(found_sustition!=function_call_sustitutions.end()) {
        found_sustition->function_code_addres = function_address;
        found_sustition->parameters_size = parameters_size;
      }

      scope_stack.emplace_back(scope_copy);
      pendind_to_generate_functions.pop_back();
    }
  }
  for (auto&sustition: function_call_sustitutions) {
    for (auto&caller_address: sustition.caller_addresses) {
      //      std::cout << "function call sustitution: " << caller_address << "->" << sustition.function_code_addres
      //                << "\n";
      write_to_byte_code(byte_code.code_data,
                         caller_address,
                         sustition.function_code_addres);
      std::cout<<"Writing parameters size:"<<sustition.parameters_size<<"\n";
      write_to_byte_code<std::uint32_t>(byte_code.code_data,
                        caller_address+sizeof(uint64_t),
                        sustition.parameters_size);
    }
  }
}
