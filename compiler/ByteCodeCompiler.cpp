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
    identifier = identifier_p.substr(0, pos);
    field_identifier = identifier_p.substr(pos + 1, identifier_p.size() - pos - 1);
  } else {
    identifier = identifier_p;
  }
  return {identifier, field_identifier};
}

std::optional<sanema::DefineFunction> get_function_definition(sanema::FunctionCall &function_call,
                                                              sanema::ByteCodeCompiler::Scope &scope);


std::optional<sanema::CompleteType>
get_variable_type(sanema::VariableEvaluation &variable, sanema::ByteCodeCompiler::Scope &scope) {
  bool is_field = is_field_identifier(variable.identifier);
  auto [identifier, field_name] = split_identifier(variable.identifier);
  if (scope.local_variables.count(std::string(identifier)) > 0) {
    if (is_field) {
      auto type = match(scope.local_variables.at(std::string(identifier)).declaration,
                        [](sanema::DeclareVariable &variable_value) {
                          return variable_value.type_identifier;
                        },
                        [](sanema::FunctionParameter &parameter) {
                          return parameter.type.value();
                        }
                       );
      return match(type,
                   [field_name, &scope](sanema::UserDefined &user_defined) -> sanema::CompleteType {
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
                   [field_name](auto &primitive) -> sanema::CompleteType {
                     throw std::runtime_error(std::format("type {} have no field {} ",
                                                          sanema::type_to_string(primitive),
                                                          field_name));
                   }
                  );
    } else {
      return match(scope.local_variables.at(std::string(identifier)).declaration,
                   [](sanema::DeclareVariable &variable_value) {
                     return variable_value.type_identifier;
                   },
                   [](sanema::FunctionParameter &parameter) {
                     return parameter.type.value();
                   }
                  );
    }
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
                   std::string message = std::format("function {}  not found",
                                                     function_call_nested.identifier);
                   throw std::runtime_error(message);
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
  if (!found_function) {
    std::string message = std::format("function {}  (",
                                      function_definition.identifier);
    std::string separator;
    for (auto &parameters: function_definition.parameters) {
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


template<class T>
T convert_number_literal(sanema::Literal literal) {
  return match(literal,
               [&](sanema::LiteralString &string) {
                 return T{};
               },
               [&](auto &literal_value) {
                 return static_cast<T>(literal_value.value);
               }
              );
}

void generate_push_temp_variable(sanema::ByteCode &byte_code,
                                 sanema::Literal &literal,
                                 sanema::ByteCodeCompiler::Scope &scope,
                                 sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
                                 sanema::CompleteType const &type,
                                 sanema::local_register_t address) {
  //TODO check if the wy we handle temp variables did changed when we consolidated the stack and operandstack
  match(literal,
        [&byte_code, &address](sanema::LiteralSInt64 int_64) {
          sanema::VMInstruction instruction{};
          instruction.opcode = OPCODE::OP_PUSH_SINT64_CONST;
          instruction.r_result = address.address;
          auto constant_id = byte_code.add_const(sanema::OperandType{int_64.value});
          instruction.registers16.r1 = constant_id;
          byte_code.write(instruction);
        },
        [&byte_code, &address](sanema::LiteralSInt32 int_32) {
          sanema::VMInstruction instruction;
          instruction.opcode = OPCODE::OP_PUSH_SINT32_CONST;
          instruction.r_result = address.address;
          auto constant_id = byte_code.add_const(sanema::OperandType{int_32.value});
          instruction.registers16.r1 = constant_id;
          byte_code.write(instruction);
        },
        [&byte_code, &address](sanema::LiteralSInt16 int_16) {
          sanema::VMInstruction instruction{};
          instruction.opcode = OPCODE::OP_PUSH_SINT16_CONST;
          instruction.r_result = address.address;
          auto constant_id = byte_code.add_const(sanema::OperandType{int_16.value});
          instruction.registers16.r1 = constant_id;
          byte_code.write(instruction);
        },
        [&byte_code, &address](sanema::LiteralSInt8 int_8) {
          sanema::VMInstruction instruction{};
          instruction.opcode = OPCODE::OP_PUSH_SINT8_CONST;
          instruction.r_result = address.address;
          auto constant_id = byte_code.add_const(sanema::OperandType{int_8.value});
          instruction.registers16.r1 = constant_id;
          byte_code.write(instruction);
        },
        [&byte_code, &address](sanema::LiteralBoolean boolean) {
          //TODO Boolean are not implemented
          sanema::VMInstruction instruction{};
          instruction.opcode = OPCODE::OP_TRUE;
          instruction.r_result = address.address;
          byte_code.write(instruction);
        },
        [&byte_code, &address](sanema::LiteralFloat a_float) {
          sanema::VMInstruction instruction{};
          instruction.opcode = OPCODE::OP_PUSH_FLOAT_CONST;
          instruction.r_result = address.address;
          auto constant_id = byte_code.add_const(sanema::OperandType{a_float.value});
          instruction.registers16.r1 = constant_id;
          byte_code.write(instruction);
        },
        [&byte_code, &address](sanema::LiteralDouble a_double) {
          sanema::VMInstruction instruction{};
          instruction.opcode = OPCODE::OP_PUSH_DOUBLE_CONST;
          instruction.r_result = address.address;
          auto constant_id = byte_code.add_const(sanema::OperandType{a_double.value});
          instruction.registers16.r1 = constant_id;
          byte_code.write(instruction);
        },
        [&byte_code, &address, type](sanema::LiteralString &string) {
          sanema::VMInstruction instruction{};
          instruction.opcode = OPCODE::OP_PUSH_STRING_CONST;
          instruction.r_result = address.address;
          auto index = byte_code.add_string_literal(string.value);
          instruction.registers16.r1 = index;
          byte_code.write(instruction);
        }
       );
}


void
generate_local_variable_access(sanema::ByteCode &byte_code, sanema::ByteCodeCompiler::Scope &context_frame_aux,
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
                    [](sanema::DeclareVariable &variable) {
                      return variable.type_identifier;
                    },
                    [](sanema::FunctionParameter &parameter) {
                      return parameter.type.value();
                    }
                   );
  bool is_external = false;

  if (is_field) {
    match_base<sanema::UserDefined &>(
      type,
      [&field_identifier, &byte_code, &is_external, &address, &type, &context_frame_aux](
        sanema::UserDefined &user_defined) {
        auto final_type = context_frame_aux.types.find_type(user_defined);
        is_external = final_type.value().external_id.has_value();
        auto field = final_type.value().get_field(field_identifier);
        if (!is_external) {
          if (field != nullptr) {
            type = field->type.value();
            address += boost::numeric_cast<std::int64_t>(field->offset);
          }

        }
      });
  }
  bool is_reference = match(local_variable_entry.declaration,
                            [](sanema::DeclareVariable &variable) {
                              return false;
                            },
                            [](sanema::FunctionParameter &parameter) {
                              return parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE ||
                                     parameter.modifier == sanema::FunctionParameter::Modifier::CONST;
                            }
                           );

  if (copy) {
    auto opcode = match(type,
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

    auto current_stack_address = context_frame_aux.scope_address;
    context_frame_aux.reserve_space_for_type(type);
    sanema::VMInstruction instruction;
    instruction.opcode = opcode;
    instruction.r_result = current_stack_address.address;
    instruction.is_r1_reference = is_reference;
    instruction.registers16.r1 = address;
    byte_code.write(instruction);
  } else {
    if (is_reference) {//is_reference Indicates if the source variable is a refernece,(the destiny is always a reference) this is the case for parameters pased by mut or const
      auto current_stack_address = context_frame_aux.scope_address;
      context_frame_aux.reserve_space_for_type(type);
      sanema::VMInstruction instruction;
      instruction.opcode = OPCODE::OP_PUSH_LOCAL_SINT64;
      instruction.r_result = current_stack_address.address;
      instruction.registers16.r1 = address;
      byte_code.write(instruction);

    } else {
      auto current_stack_address = context_frame_aux.scope_address;
      context_frame_aux.reserve_space_for_type(type);
      sanema::VMInstruction instruction;
      instruction.opcode = OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL;
      instruction.r_result = current_stack_address.address;
      instruction.registers16.r1 = address;
      byte_code.write(instruction);

    }
  }
}


void
generate_set(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
             std::vector<sanema::local_register_t> addresses ,sanema::local_register_t return_address) {
  auto opcode = match(function_definition->type,
                      [](sanema::Integer const &integer) -> OPCODE {
                        switch (integer.size) {
                          case 8:
                            return OPCODE::OP_SET_LOCAL_SINT8;
                          case 16:
                            return OPCODE::OP_SET_LOCAL_SINT16;
                          case 32:
                            return OPCODE::OP_SET_LOCAL_SINT32;
                          case 64:
                            return OPCODE::OP_SET_LOCAL_SINT64;
                        }
                        return  OPCODE::OP_SET_LOCAL_SINT64;
                      },
                      [](sanema::Float const &a_float) -> OPCODE {
                        return OPCODE::OP_SET_LOCAL_FLOAT;
                      },
                      [](sanema::String const &integer) -> OPCODE  {
                        return OPCODE::OP_SET_LOCAL_STRING;
                      },
                      [](sanema::Void const &a_void) -> OPCODE {
                        throw std::runtime_error("Void can't be set we should never reach this");
                      },
                      [](sanema::Double const &a_double) -> OPCODE {
                        return OPCODE::OP_SET_LOCAL_DOUBLE;
                      },
                      [](sanema::Boolean const &integer) -> OPCODE {
                        return OPCODE::OP_SET_LOCAL_BOOL;
                      },
                      [](sanema::UserDefined const &integer) -> OPCODE {
                        throw std::runtime_error("User defined types can't be set we should never reach this");
                      }
                     );
  sanema::VMInstruction instruction;
  instruction.opcode = opcode;
  instruction.is_rresult_reference=addresses[0].is_reference;
  instruction.r_result = addresses[0].address;
  instruction.is_r1_reference=addresses[1].is_reference;
  instruction.registers16.r1 = addresses[1].address;
  byte_code.write(instruction);
}

void
generate_return(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                [[maybe_unused]] std::vector<sanema::local_register_t> addresses , sanema::local_register_t return_address) {
  sanema::VMInstruction instruction;
  instruction.opcode = OPCODE::OP_RETURN;
  byte_code.write(instruction);
}

std::optional<sanema::DefineFunction>
generate_function_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions
);

std::optional<sanema::DefineFunction> generate_operator_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions,
  uint16_t address
);

std::optional<sanema::DefineFunction> generate_function_or_operator_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions,
  uint16_t address
) {
  sanema::DefineFunction function_definition;
  function_definition.identifier = function_call.identifier;
  auto final_function_definition = get_function_definition(function_call,
                                                           context_frame_aux);
  if (!final_function_definition.has_value()) {
    throw std::runtime_error("can't generate function " + function_call.identifier);
  }
  if (final_function_definition->is_operator) {
    return generate_operator_call(byte_code,
                                  function_call,
                                  context_frame_aux,
                                  generator_map,
                                  function_call_sustitutions,
                                  address);
  } else {
    return generate_function_call(byte_code,
                                  function_call,
                                  context_frame_aux,
                                  generator_map,
                                  function_call_sustitutions);
  }

}

void generate_expression_access(
  sanema::Expression const &expression,
  sanema::FunctionParameter::Modifier modifier,
  sanema::CompleteType const &type,
  sanema::ByteCode &byte_code,
  sanema::ByteCodeCompiler::Scope &scope,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions,
  sanema::local_register_t address
) {
  match(expression,
        [&](sanema::Literal literal) -> void {
          auto literal_type = sanema::get_literal_type(literal);
          if (modifier == sanema::FunctionParameter::Modifier::MUTABLE) {
            throw std::runtime_error("can't bind literal to a mutable reference");
          }


          auto address = scope.scope_address;
          scope.reserve_space_for_type(type);
          generate_push_temp_variable(byte_code,
                                      literal,
                                      scope,
                                      generator_map,
                                      literal_type,
                                      address);
        },
        [&](sanema::FunctionCall function_call_nested) -> void {
          if (modifier == sanema::FunctionParameter::Modifier::MUTABLE ||
              modifier == sanema::FunctionParameter::Modifier::CONST) {
            throw std::runtime_error("can't bind temporary value  to a  reference");
          }
          auto current_address = scope.scope_address;
          auto definition = generate_function_or_operator_call(byte_code,
                                                               function_call_nested,
                                                               scope,
                                                               generator_map,
                                                               function_call_sustitutions,
                                                               current_address.address);
          if (!definition) {
            //We should not reach this because the get_function_definition at the start of the function should validate this
            throw std::runtime_error(std::format(
              "function {} not found, but that is unexpected",
              function_call_nested.identifier));
          }
        },
        [&](sanema::VariableEvaluation variable_evaluation) -> void {
          auto variable_type = get_variable_type(variable_evaluation,
                                                 scope);
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
                                         scope,
                                         variable_evaluation.identifier,
                                         should_copy);
        });
}


std::optional<sanema::DefineFunction> generate_operator_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions,
  uint16_t address
) {
  sanema::DefineFunction function_definition;
  function_definition.identifier = function_call.identifier;
  auto final_function_definition = get_function_definition(function_call,
                                                           context_frame_aux);
  if (!final_function_definition.has_value()) {
    throw std::runtime_error("can't generate function " + function_call.identifier);
  }
  auto return_address = context_frame_aux.scope_address;
//  context_frame_aux.reserve_space_for_type(final_function_definition->type);
  sanema::ByteCodeCompiler::Scope context_frame_aux_copy=context_frame_aux;
  std::vector<sanema::local_register_t> parameter_addresses;
  parameter_addresses.reserve(function_call.arguments.size());
  for (int i = 0; i < final_function_definition.value().parameters.size(); i++) {
    auto &argument = function_call.arguments[i];
    auto &parameter = final_function_definition.value().parameters[i];
    match(argument.expression,
          [&](sanema::Literal literal) -> void {
            auto literal_type = sanema::get_literal_type(literal);
            if (parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE) {
              throw std::runtime_error("can't bind literal to a mutable reference");
            }
            auto address_temporal = context_frame_aux.scope_address;
            parameter_addresses.emplace_back(address_temporal);
            context_frame_aux.reserve_space_for_type(parameter.type.value());
            generate_push_temp_variable(byte_code,
                                        literal,
                                        context_frame_aux,
                                        generator_map,
                                        literal_type,
                                        address_temporal);

          },
          [&](sanema::FunctionCall function_call_nested) -> void {
            if (parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE ||
                parameter.modifier == sanema::FunctionParameter::Modifier::CONST) {
              throw std::runtime_error("can't bind temporary value  to a  reference");
            }
            auto address_return = context_frame_aux.scope_address;
            if (function_call.identifier == "return") {
              address_return.address = 0;
            }

            parameter_addresses.emplace_back(address_return);
//            context_frame_aux.reserve_space_for_type(parameter.type.value());

            auto definition = generate_function_or_operator_call(byte_code,
                                                                 function_call_nested,
                                                                 context_frame_aux_copy,
                                                                 generator_map,
                                                                 function_call_sustitutions,
                                                                 (uint64_t) address_return.address);

            if (!definition) {
              //We should not reach this because the get_function_definition at the start of the function should validate this
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
            auto local_variable_entry = context_frame_aux.local_variables.at(variable_evaluation.identifier);
            sanema::local_register_t address_variable;
            bool is_reference = match(local_variable_entry.declaration,
                            [](sanema::DeclareVariable &variable) {
                              return false;
                            },
                            [](sanema::FunctionParameter &parameter) {
                              return parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE ||
                                     parameter.modifier == sanema::FunctionParameter::Modifier::CONST;
                            }
                           );


            address_variable.address = local_variable_entry.address;
            address_variable.is_reference=is_reference;
            if (function_call.identifier == "return") {
              sanema::VMInstruction instruction;
              instruction.opcode = OPCODE::OP_PUSH_LOCAL_SINT64;
              instruction.r_result = 0;
              instruction.registers16.r1 = address_variable.address;
              byte_code.write(instruction);
            }
            parameter_addresses.emplace_back(address_variable);
          });
  }

  if (generator_map.map.count(function_call.identifier) > 0) {
    generator_map.map.at(function_call.identifier)(byte_code,
                                                   final_function_definition,
                                                   parameter_addresses,
                                                   sanema::local_register_t{address});
  } else {
    throw std::runtime_error(" generator not found ");
  }
  return final_function_definition;
}

std::optional<sanema::DefineFunction>
generate_function_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions
) {
  sanema::DefineFunction function_definition;
  function_definition.identifier = function_call.identifier;
  auto final_function_definition = get_function_definition(function_call,
                                                           context_frame_aux);
  if (!final_function_definition.has_value()) {
    throw std::runtime_error("can't generate function " + function_call.identifier);
  }
  auto return_address = context_frame_aux.scope_address;
  context_frame_aux.reserve_space_for_type(final_function_definition->type);
  auto contex_frame_aux_copy=context_frame_aux;
  auto rollback_address=context_frame_aux.scope_address;
  for (int i = 0; i < final_function_definition.value().parameters.size(); i++) {
    auto &argument = function_call.arguments[i];
    auto &parameter = final_function_definition.value().parameters[i];
    match(argument.expression,
          [&](sanema::Literal literal) -> void {
            auto literal_type = sanema::get_literal_type(literal);
            if (parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE || parameter.modifier == sanema::FunctionParameter::Modifier::CONST) {
              //TODO to allow binding of literals to references we need to properly implement temporaries or a method to flat out the call tree
              throw std::runtime_error("can't bind literal to a a reference");
            }


            auto address = contex_frame_aux_copy.scope_address;
            contex_frame_aux_copy.reserve_space_for_type(parameter.type.value());
            generate_push_temp_variable(byte_code,
                                        literal,
                                        contex_frame_aux_copy,
                                        generator_map,
                                        literal_type,
                                        address);
          },
          [&](sanema::FunctionCall function_call_nested) -> void {
            if (parameter.modifier == sanema::FunctionParameter::Modifier::MUTABLE || parameter.modifier == sanema::FunctionParameter::Modifier::CONST) {
              throw std::runtime_error("can't bind temporary value  to a  reference");
            }
            auto return_address = contex_frame_aux_copy.scope_address;
            auto definition = generate_function_or_operator_call(byte_code,
                                                                 function_call_nested,
                                                                 contex_frame_aux_copy,
                                                                 generator_map,
                                                                 function_call_sustitutions,
                                                                 std::uint64_t(return_address.address));
            if (!definition) {
              //We should not reach this because the get_function_definition at the start of the function should validate this
              throw std::runtime_error(std::format(
                "function {} not found, but that is unexpected",
                function_call_nested.identifier));
            }
          },
          [&](sanema::VariableEvaluation variable_evaluation) -> void {
            auto variable_type = get_variable_type(variable_evaluation,
                                                   contex_frame_aux_copy);
            if (!variable_type.has_value()) {
              throw std::runtime_error(std::format("variable  {} not found ",
                                                   variable_evaluation.identifier));
            }
            bool should_copy = false;
            switch (parameter.modifier) {
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
                                           contex_frame_aux_copy,
                                           variable_evaluation.identifier,
                                           should_copy);
          });
  }
  context_frame_aux.scope_address=rollback_address;
  if (final_function_definition->external_id) {
    sanema::VMInstruction instruction;
    instruction.opcode = OPCODE::OP_CALL_EXTERNAL_FUNCTION;
    instruction.register32.r1 = final_function_definition->external_id.value();
    instruction.r_result = return_address.address;
    byte_code.write(instruction);;
  } else {
    sanema::VMInstruction instruction;
    instruction.opcode = OPCODE::OP_CALL;
    instruction.r_result = return_address.address;
    auto address = byte_code.write(instruction);;
    auto sustitution_iter = std::find_if(function_call_sustitutions.begin(),
                                         function_call_sustitutions.end(),
                                         [&final_function_definition](
                                           sanema::ByteCodeCompiler::FuctionCallSustitution &sustitution) {
                                           return sustitution.function_id == final_function_definition->id;
                                         });
    if (sustitution_iter == function_call_sustitutions.end()) {
      function_call_sustitutions.emplace_back(std::vector{address},
                                              0,
                                              0,
                                              final_function_definition->id);
    } else {
      sustitution_iter->caller_addresses.emplace_back(address);
    }
  }
  return final_function_definition;
}


void sanema::ByteCodeCompiler::Scope::reserve_space_for_type(CompleteType const &type) {
  auto size = get_type_size(type);
  scope_address.address += size;
}


std::uint32_t
sanema::ByteCodeCompiler::generate_block(sanema::BlockOfCode &block_of_code, FunctionCollection &built_in_functions,
                                         TypeCollection &external_types) {
  std::int64_t total_variable_space = 0;

  for (auto &instruction: block_of_code.instructions) {
    match(instruction.instruction_sum,
          [this](DefineStruct &define_struct) {
            auto &scope = scope_stack.back();
            if (!define_struct.user_type.has_value()) {
              throw std::runtime_error("user type has no value ");
            }

            auto identifier = define_struct.user_type.value().type_id.identifier;
            if (!scope.types.containts(identifier)) {
              std::uint64_t offset = 0;
              for (auto &field: define_struct.user_type->fields) {
                field.offset = offset;
                offset += get_type_size(field.type.value());
              }
              scope.types.add_type(define_struct.user_type.value());
            }
          },
          [&](IfStatement &if_statement) {
            auto current_scope = scope_stack.back();
            auto current_stack_address = current_scope.scope_address;
            generate_expression_access(if_statement.expression,
                                       FunctionParameter::Modifier::VALUE,
                                       CompleteType{sanema::Boolean{}},
                                       byte_code,
                                       current_scope,
                                       function_bytecode_generators,
                                       function_call_sustitutions,
                                       current_stack_address);
            VMInstruction jump_if_false;
            jump_if_false.opcode = OPCODE::OP_JUMP_IF_FALSE;
            jump_if_false.registers16.r2 = current_stack_address.address;
            std::uint64_t address_false_jump = byte_code.write(jump_if_false);
            auto address_true_branch = byte_code.get_current_address();
            generate_block(if_statement.true_path,
                           built_in_functions,
                           external_types);

            VMInstruction jump_unconditional;
            jump_unconditional.opcode = OPCODE::OP_JUMP;
            jump_unconditional.registers16.r1 = 0;
            std::uint64_t address_true_jump_instruction = byte_code.write(jump_unconditional);
            auto address_false_branch = byte_code.get_current_address();

            generate_block(if_statement.false_path,
                           built_in_functions,
                           external_types);
            auto address_end_if = byte_code.get_current_address();

            byte_code.code_data[address_false_jump].registers16.r1 = address_false_branch-address_true_branch;
            byte_code.code_data[address_true_jump_instruction].registers16.r1 = address_end_if-address_false_branch;
          },
          [this, &total_variable_space](DeclareVariable &declare_variable) {
            auto &current_scope = scope_stack.back();
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
                for (auto &field: final_type.value().fields) {
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
              } else {
                throw std::runtime_error(std::format("Type {} does not exists",
                                                     user_defined.type_id.identifier));
              }
            } else {
              total_variable_space += get_type_size(declare_variable.type_identifier);
//              FunctionCall function_call;
//              function_call.identifier = "set";
//              function_call.arguments.emplace_back(VariableEvaluation{declare_variable.identifier});
//              function_call.arguments.emplace_back(get_default_literal_for_type(declare_variable.type_identifier));
//              generate_function_call(byte_code,
//                                     function_call,
//                                     current_scope,
//                                     function_bytecode_generators,
//                                     function_call_sustitutions);
            }
          },
          [this](DefineFunction &function) {
            auto function_entry = scope_stack.back().function_collection.find_function(function);
            if (!function_entry) {
              auto id = scope_stack.back().function_collection.add_function(function);
              pendind_to_generate_functions.emplace_back(id);
            } else {
              throw std::runtime_error(std::format("function {} already defined",
                                                   function.identifier));
            }
          },
          [this](FunctionCall &function_call) {
            auto address = scope_stack.back().scope_address;


            generate_function_or_operator_call(byte_code,
                                               function_call,
                                               scope_stack.back(),
                                               function_bytecode_generators,
                                               function_call_sustitutions,
                                               (std::uint64_t) address.address);
          },
          [&](BlockOfCode &block_of_code) {
            total_variable_space += generate_block(block_of_code,
                                                   built_in_functions,
                                                   external_types);
          }
         );
  }


  return total_variable_space;
}

void sanema::ByteCodeCompiler::process(sanema::BlockOfCode &block_of_code, FunctionCollection &built_in_functions,
                                       TypeCollection &external_types) {
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
//    byte_code.write(OPCODE::OP_RESERVE_STACK_SPACE);
//    auto addres_variable_space = byte_code.write<std::uint64_t>(0);
    auto total_variable_space = generate_block(next_block.value(),
                                               built_in_functions,
                                               external_types);
    VMInstruction return_instruction;
    return_instruction.opcode = OPCODE::OP_RETURN;
    byte_code.write(return_instruction);
//    byte_code.write_to_address<std::uint64_t>(total_variable_space,
//                                              addres_variable_space);
    next_block = {};
    if (!pendind_to_generate_functions.empty()) {
      auto pending_function = pendind_to_generate_functions.back();
      auto &scope = scope_stack.back();
      auto function = scope.function_collection.get_function_by_id(pending_function);
      if (function == nullptr) {
        throw std::runtime_error("can't find function , this is unexpected, maybe a bug in the compiler");
      }

      // ReSharper disable once CppDFANullDereference
      next_block = function->body;
      std::uint64_t function_address = byte_code.get_current_address();
      function->address= function_address;
      auto scope_copy = scope;


      //      std::cout << "Determining function address : " << function_address << "\n";

      scope_copy.scope_address.address = 0;
      std::int64_t parameter_address = 0;
      scope_copy.local_variables.clear();
      std::int32_t parameters_size = get_type_size(function->type);
      scope_copy.reserve_space_for_type(function->type);
      for (auto &parameter: function->parameters) {
        parameter_address = parameters_size;
        if (parameter.modifier == FunctionParameter::Modifier::CONST ||
            parameter.modifier == FunctionParameter::Modifier::MUTABLE) {
          parameters_size += boost::numeric_cast<std::int64_t>(get_type_size(Integer(64)));;
          scope_copy.reserve_space_for_type(Integer(64));
        } else {
          parameters_size += boost::numeric_cast<std::int64_t>(get_type_size(parameter.type.value()));
          scope_copy.reserve_space_for_type(parameter.type.value());
        }
        scope_copy.local_variables.emplace(parameter.identifier,
                                           VariableEntry{parameter, parameter_address});

      }
      auto found_sustition = std::find_if(function_call_sustitutions.begin(),
                                          function_call_sustitutions.end(),
                                          [&pending_function, function_address, &parameters_size](
                                            FuctionCallSustitution &sustitution) {
                                            return sustitution.function_id == pending_function;
                                          });
      if (found_sustition != function_call_sustitutions.end()) {
        found_sustition->function_code_addres = function_address;
        found_sustition->parameters_size = parameters_size;
      }

      scope_stack.emplace_back(scope_copy);
      pendind_to_generate_functions.pop_back();
    }
  }
  for (auto &sustition: function_call_sustitutions) {
    for (auto &caller_address: sustition.caller_addresses) {
      byte_code.code_data[caller_address].register32.r1 = sustition.function_code_addres;
    }
  }
  byte_code.function_collection=scope_stack.back().function_collection;
}
