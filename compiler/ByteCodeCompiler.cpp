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

sanema::CompleteType query_final_type(sanema::IncompleteType& type, sanema::TypeCollection& types) {
  return match(type,
         [&types](sanema::UnidentifiedType& user_defined)->sanema::CompleteType {
           auto final_type = types.find_type(user_defined);
           if (!final_type.has_value()) {
             throw std::runtime_error(std::format("user defined type {} do not exists  ",
                                                   user_defined.type_id.identifier));
           }
           return final_type.value();
         },
         [](auto& primitive)-> sanema::CompleteType{
           return primitive;
         }
    );
}
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

std::optional<sanema::FunctionDefinitionCompleted> get_function_definition(sanema::FunctionCall &function_call,
                                                              sanema::ByteCodeCompiler::Scope &scope);


std::optional<sanema::CompleteType>
get_variable_type(sanema::VariableEvaluation &variable, sanema::ByteCodeCompiler::Scope &scope) {
  bool is_field = is_field_identifier(variable.identifier);
  auto [identifier, field_name] = split_identifier(variable.identifier);
  if (scope.local_variables.count(std::string(identifier)) > 0) {
    if (is_field) {
      auto type = match(scope.local_variables.at(std::string(identifier)).declaration,
                        [&scope](sanema::DeclareVariable &variable_value)->sanema::CompleteType {
                          return query_final_type(variable_value.type_identifier,scope.types);
                        },
                        [](sanema::FunctionParameterCompleted &parameter)->sanema::CompleteType {
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
                   [&scope](sanema::DeclareVariable &variable_value)->sanema::CompleteType {
                     return query_final_type(variable_value.type_identifier,scope.types);
                   },
                   [&scope](sanema::FunctionParameterCompleted &parameter)->sanema::CompleteType {
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

std::optional<sanema::FunctionDefinitionCompleted> get_function_definition(sanema::FunctionCall &function_call,
                                                              sanema::ByteCodeCompiler::Scope &scope) {
  sanema::FunctionDefinitionCompleted function_definition;
  function_definition.identifier = function_call.identifier;
  for (auto &argument: function_call.arguments) {
    auto type = get_expression_type(argument.expression,
                                    scope);
    if (!type.has_value()) return {};
    function_definition.parameters.emplace_back(sanema::FunctionParameterCompleted{"", {}, type.value()});
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
                                 sanema::Literal literal,
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

std::tuple<std::int64_t,sanema::ByteCodeCompiler::VariableEntry> get_local_variable_address(sanema::ByteCode &byte_code, sanema::ByteCodeCompiler::Scope &context_frame_aux,
                               std::string identifier_p) {
  bool is_field = is_field_identifier(identifier_p);
  auto [identifier, field_identifier] = split_identifier(identifier_p);
  std::string temp_identifier = std::string(identifier);
  auto local_variable_entry = context_frame_aux.local_variables.at(temp_identifier);
  auto type = match(local_variable_entry.declaration,
                    [&context_frame_aux](sanema::DeclareVariable &variable)->sanema::CompleteType {
                      return query_final_type(variable.type_identifier,context_frame_aux.types);
                    },
                    [&context_frame_aux](sanema::FunctionParameterCompleted &parameter)->sanema::CompleteType {
                      return parameter.type.value();
                    }
                    );
  bool is_external = false;
  std::int64_t address = local_variable_entry.address;
  if (is_field) {
    match_base<sanema::UserDefined &>(
      type,
      [&field_identifier, &byte_code, &is_external, &address, &type, &context_frame_aux](
        sanema::UserDefined &user_defined) {
        auto final_type = context_frame_aux.types.find_type(user_defined);
        is_external = final_type.value().external_id.has_value();
        auto field = final_type.value().get_field(field_identifier);
        // if (!is_external) {
          if (field != nullptr) {
            type = field->type.value();
            // std::cout<<"generating variable access for "<<field_identifier<<" of type "<<sanema::type_to_string(type)<<"\n";
            // std::cout<<"offset: "<<field->offset<<"\n";
            address += boost::numeric_cast<std::int64_t>(field->offset);
          }

        // }
      });
  }
  return {address,local_variable_entry};

}
void
generate_local_variable_access(sanema::ByteCode &byte_code, sanema::ByteCodeCompiler::Scope &context_frame_aux,
                               std::string identifier_p,
                               bool copy) {
  auto pos = identifier_p.find('.');


  std::vector<std::string> accesors_list;
  boost::split(accesors_list,
               identifier_p,
               boost::is_any_of("."));
 auto [address,local_variable_entry]=   get_local_variable_address(byte_code,context_frame_aux,identifier_p);

  auto type = match(local_variable_entry.declaration,
                    [&context_frame_aux](sanema::DeclareVariable &variable) {
                      return query_final_type(variable.type_identifier,context_frame_aux.types);
                    },
                    [](sanema::FunctionParameterCompleted &parameter) {
                      return parameter.type.value();
                    }
                   );



  bool is_reference = match(local_variable_entry.declaration,
                            [](sanema::DeclareVariable &variable) {
                              return false;
                            },
                            [](sanema::FunctionParameterCompleted &parameter) {
                              return parameter.modifier == sanema::FunctionParameterCompleted::Modifier::MUTABLE ||
                                     parameter.modifier == sanema::FunctionParameterCompleted::Modifier::CONST;
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
                        [](sanema::UserDefined &integer) {
                          return OPCODE::OP_COPY_MEMORY;
                        },
                        [](auto &User) {
                          return OPCODE::OP_PUSH_LOCAL_SINT64;
                        });
    auto type_size=sanema::get_type_size(type);
    auto current_stack_address = context_frame_aux.scope_address;
    context_frame_aux.reserve_space_for_type(type);
    sanema::VMInstruction instruction;
    instruction.opcode = opcode;
    instruction.r_result = current_stack_address.address;
    instruction.is_r1_reference = is_reference;
    instruction.registers16.r1 = address;
    instruction.registers16.r2 = type_size;
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
generate_set(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
             std::vector<sanema::local_register_t> addresses ,sanema::local_register_t return_address) {
  auto opcode = match(function_definition->parameters[0].type.value(),
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

std::optional<sanema::FunctionDefinitionCompleted>
generate_function_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions
);

std::optional<sanema::FunctionDefinitionCompleted> generate_operator_call(
  sanema::ByteCode &byte_code,
  sanema::FunctionCall &function_call,
  sanema::ByteCodeCompiler::Scope &context_frame_aux,
  sanema::ByteCodeCompiler::GeneratorsMap &generator_map,
  std::vector<sanema::ByteCodeCompiler::FuctionCallSustitution> &function_call_sustitutions,
  uint16_t address
);

std::optional<sanema::FunctionDefinitionCompleted> generate_function_or_operator_call(
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
  sanema::FunctionParameterIncomplete::Modifier modifier,
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
          if (modifier == sanema::FunctionParameterIncomplete::Modifier::MUTABLE) {
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
          if (modifier == sanema::FunctionParameterIncomplete::Modifier::MUTABLE ||
              modifier == sanema::FunctionParameterIncomplete::Modifier::CONST) {
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
            case sanema::FunctionParameterIncomplete::Modifier::MUTABLE:
              should_copy = false;
              break;
            case sanema::FunctionParameterIncomplete::Modifier::CONST:
              should_copy = false;
              break;
            case sanema::FunctionParameterIncomplete::Modifier::VALUE:
              should_copy = true;
              break;
          }
          generate_local_variable_access(byte_code,
                                         scope,
                                         variable_evaluation.identifier,
                                         should_copy);
        });
}


std::optional<sanema::FunctionDefinitionCompleted> generate_operator_call(
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
  context_frame_aux.reserve_space_for_type(final_function_definition->type);
  sanema::ByteCodeCompiler::Scope context_frame_aux_copy=context_frame_aux;
  std::vector<sanema::local_register_t> parameter_addresses;
  parameter_addresses.reserve(function_call.arguments.size());
  for (int i = 0; i < final_function_definition.value().parameters.size(); i++) {
    auto &argument = function_call.arguments[i];
    auto &parameter = final_function_definition.value().parameters[i];
    match(argument.expression,
          [&](sanema::Literal literal) -> void {
            auto literal_type = sanema::get_literal_type(literal);
            if (parameter.modifier == sanema::FunctionParameterCompleted::Modifier::MUTABLE) {
              throw std::runtime_error("can't bind literal to a mutable reference");
            }
            auto address_temporal = context_frame_aux_copy.scope_address;
            if (function_call.identifier == "return") {
              address_temporal.address = 0;
            }
            parameter_addresses.emplace_back(address_temporal);
            context_frame_aux_copy.reserve_space_for_type(parameter.type.value());
            generate_push_temp_variable(byte_code,
                                        literal,
                                        context_frame_aux_copy,
                                        generator_map,
                                        literal_type,
                                        address_temporal);

          },
          [&](sanema::FunctionCall function_call_nested) -> void {
            if (parameter.modifier == sanema::FunctionParameterCompleted::Modifier::MUTABLE ||
                parameter.modifier == sanema::FunctionParameterCompleted::Modifier::CONST) {
              throw std::runtime_error("can't bind temporary value  to a  reference");
            }
            auto address_return = context_frame_aux_copy.scope_address;


            parameter_addresses.emplace_back(address_return);
//            context_frame_aux.reserve_space_for_type(parameter.type.value());

            auto definition = generate_function_or_operator_call(byte_code,
                                                                 function_call_nested,
                                                                 context_frame_aux_copy,
                                                                 generator_map,
                                                                 function_call_sustitutions,
                                                                 (uint64_t) address_return.address);

             if (function_call.identifier == "return") {
              sanema::VMInstruction instruction;
              instruction.opcode = OPCODE::OP_PUSH_LOCAL_SINT64;
              instruction.r_result = 0;
              instruction.registers16.r1 = address_return.address;
              byte_code.write(instruction);
            }

            if (!definition) {
              //We should not reach this because the get_function_definition at the start of the function should validate this
              throw std::runtime_error(std::format(
                "function {} not found, but that is unexpected",
                function_call_nested.identifier));
            }
          },
          [&](sanema::VariableEvaluation variable_evaluation) -> void {
            auto variable_type = get_variable_type(variable_evaluation,
                                                   context_frame_aux_copy);
            if (!variable_type.has_value()) {
              throw std::runtime_error(std::format("variable  {} not found ",
                                                   variable_evaluation.identifier));
            }
            auto  [address,local_variable_entry] =get_local_variable_address(byte_code,context_frame_aux_copy,variable_evaluation.identifier);
            sanema::local_register_t address_variable;
            bool is_reference = match(local_variable_entry.declaration,
                            [](sanema::DeclareVariable &variable) {
                              return false;
                            },
                            [](sanema::FunctionParameterCompleted &parameter) {
                              return parameter.modifier == sanema::FunctionParameterCompleted::Modifier::MUTABLE ||
                                     parameter.modifier == sanema::FunctionParameterCompleted::Modifier::CONST;
                            }
                           );


            address_variable.address = address;
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

std::optional<sanema::FunctionDefinitionCompleted> generate_function_call(
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
  auto call_offset_address = context_frame_aux.scope_address;
  context_frame_aux.reserve_space_for_type(final_function_definition->type);
  auto contex_frame_aux_copy=context_frame_aux;
  auto rollback_address=context_frame_aux.scope_address;
  for (int i = 0; i < final_function_definition.value().parameters.size(); i++) {
    auto &argument = function_call.arguments[i];
    auto &parameter = final_function_definition.value().parameters[i];
    match(argument.expression,
          [&](sanema::Literal literal) -> void {
            auto literal_type = sanema::get_literal_type(literal);
            if (parameter.modifier == sanema::FunctionParameterCompleted::Modifier::MUTABLE || parameter.modifier == sanema::FunctionParameterCompleted::Modifier::CONST) {
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
            if (parameter.modifier == sanema::FunctionParameterCompleted::Modifier::MUTABLE || parameter.modifier == sanema::FunctionParameterCompleted::Modifier::CONST) {
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
              case sanema::FunctionParameterCompleted::Modifier::MUTABLE:
                should_copy = false;
                break;
              case sanema::FunctionParameterCompleted::Modifier::CONST:
                should_copy = false;
                break;
              case sanema::FunctionParameterCompleted::Modifier::VALUE:
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
    instruction.r_result = call_offset_address.address;
    byte_code.write(instruction);;
  } else {
    sanema::VMInstruction instruction;
    // std::cout<<"generating calling function :"<<function_call.identifier<<"\n";
    instruction.opcode = OPCODE::OP_CALL;
    instruction.r_result = call_offset_address.address;
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
  std::int64_t total_variable_space = scope_stack.back().scope_address.address;
  for (auto &instruction: block_of_code.instructions) {
    match(instruction.instruction_sum,
          [this](DefineStruct &define_struct) {
            auto &scope = scope_stack.back();
            UserDefined user_defined{define_struct.type_id};
            auto identifier = define_struct.type_id.identifier;
            if (!scope.types.containts(identifier)) {
              std::uint64_t offset = 0;
              std::uint64_t type_size = 0;

              for (auto &field: define_struct.fields) {
                if(!field.type.has_value()) {
                  throw std::runtime_error("field type has no value ");
                }
                auto type=query_final_type(field.type.value(), scope.types);
                Field complete_field{field.identifier,type};
                complete_field.offset= offset;
                user_defined.fields.emplace_back(complete_field);
                offset += get_type_size(type);
                type_size=offset;
              }
              user_defined.size = type_size;
              scope.types.add_type(user_defined);
            }
          },
          [this, &built_in_functions, &external_types](ForStatement &for_statement) {
            auto &current_scope = scope_stack.back();
            auto initial_scope_address = current_scope.scope_address; // Save current stack pointer
            auto limit_for_address = current_scope.scope_address; // Save current stack pointer

            // 1. Evaluate the loop count expression (e.g., '10')
            // The result will be pushed to current_scope.scope_address
            generate_expression_access(for_statement.expression,
                                       FunctionParameterIncomplete::Modifier::VALUE,
                                       CompleteType{sanema::Integer{64}}, // Assuming loop count is a 64-bit integer
                                       byte_code,
                                       current_scope,
                                       function_bytecode_generators,
                                       function_call_sustitutions,
                                       current_scope.scope_address);






            // 2. Declare and initialize the loop index variable (e.g., 'i')
            if (current_scope.local_variables.count(for_statement.identifier) != 0) {
                throw std::runtime_error("Loop index variable " + for_statement.identifier + " already defined");
            }

            sanema::local_register_t loop_index_reg = current_scope.scope_address;
            std::cout<<"address before for variable="<<current_scope.scope_address.address<<"\n";
            current_scope.local_variables.emplace(for_statement.identifier,
                                                  VariableEntry{DeclareVariable(for_statement.identifier, Integer{64}), loop_index_reg.address});
            current_scope.reserve_space_for_type(CompleteType{sanema::Integer{64}});
            std::cout<<"address after for variable="<<current_scope.scope_address.address<<"\n";
            //Set index variable to 0
              sanema::FunctionCall set_call_limit;
            set_call_limit.identifier = "set";
            set_call_limit.arguments.emplace_back(FunctionArgument{VariableEvaluation{for_statement.identifier}}); // Placeholder for destination
            set_call_limit.arguments.emplace_back(FunctionArgument{LiteralSInt64{0}}); // Placeholder for source
            generate_operator_call(byte_code,set_call_limit,current_scope,function_bytecode_generators,function_call_sustitutions,current_scope.scope_address.address);


            // 3. Loop structure
            std::uint64_t loop_start_address = byte_code.get_current_address();



            sanema::local_register_t condition_result_reg = current_scope.scope_address;

            // Generate 'less' operation (index < limit)
            sanema::VMInstruction less_instruction;
            less_instruction.opcode = OPCODE::OP_LESS_SINT64;
            less_instruction.r_result = condition_result_reg.address; // Result (boolean)
            less_instruction.registers16.r1 = loop_index_reg.address; // First operand (loop_index)
            less_instruction.registers16.r2 = limit_for_address.address; // Second operand (loop_limit)
            byte_code.write(less_instruction);
            current_scope.reserve_space_for_type(CompleteType{sanema::Boolean{}});

           std::cout<<"address after less comp="<<current_scope.scope_address.address<<"\n";
            // Jump if condition false
            VMInstruction jump_if_false;
            jump_if_false.opcode = OPCODE::OP_JUMP_IF_FALSE;
            jump_if_false.registers16.r2 = condition_result_reg.address;
            std::uint64_t address_jump_to_end = byte_code.write(jump_if_false); // Placeholder for jump target

            auto for_body_address=byte_code.get_current_address();
            // Generate loop body
            auto block_variable_offset=generate_block(for_statement.body, built_in_functions, external_types);
           current_scope.scope_address.address+=block_variable_offset;

            // Push 1 (constant)
            auto one_literal_temp = current_scope.scope_address;

            generate_push_temp_variable(byte_code,
                                        sanema::LiteralSInt64{1},
                                        current_scope,
                                        function_bytecode_generators,
                                        CompleteType{sanema::Integer{64}},
                                        one_literal_temp);
            current_scope.reserve_space_for_type(CompleteType{sanema::Integer{64}});

            // Generate 'add' operation (index + 1)
            sanema::VMInstruction add_instruction;
            add_instruction.opcode = OPCODE::OP_ADD_SINT64;
            add_instruction.r_result = loop_index_reg.address ; // Result of addition
            add_instruction.registers16.r1 = loop_index_reg.address; // First operand (loop_index)
            add_instruction.registers16.r2 = one_literal_temp.address; // Second operand (1)
            std::print(std::cout," generating add Rresult {} r1 {} ,r2 {}\n",loop_index_reg.address, add_instruction.registers16.r1, add_instruction.registers16.r2 );
            byte_code.write(add_instruction);
            current_scope.reserve_space_for_type(CompleteType{sanema::Integer{64}});
            sanema::local_register_t new_index_value_temp = current_scope.scope_address;

            // Unconditional jump back to loop start
            auto address_jump_to_begin=byte_code.get_current_address();
            VMInstruction jump_to_start;
            jump_to_start.opcode = OPCODE::OP_JUMP_BACK;
            jump_to_start.registers16.r1 = 0; // Jump target
            byte_code.write(jump_to_start);

            // Loop end address
            std::uint64_t loop_end_address = byte_code.get_current_address();

            // Patch the jump_if_false instruction
            byte_code.code_data[address_jump_to_end].registers16.r1 = loop_end_address-address_jump_to_end;
            byte_code.code_data[address_jump_to_begin].registers16.r1 = (address_jump_to_begin + 1) - loop_start_address;

            // Restore scope address (pop loop_limit_reg and loop_index_reg)
            current_scope.scope_address = initial_scope_address;
            // Remove loop index variable from scope
            current_scope.local_variables.erase(for_statement.identifier);
          },
          [&](IfStatement &if_statement) {
            auto current_scope = scope_stack.back();
            auto current_stack_address = current_scope.scope_address;
            generate_expression_access(if_statement.expression,
                                       FunctionParameterIncomplete::Modifier::VALUE,
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
          [&](ReturnStatement &return_statement) {
            //TODO right now returning a value  generate two copies we need to optimize this, we may need to change generate expression access for this
            auto current_scope = scope_stack.back();
            auto current_stack_address = current_scope.scope_address;
            auto return_type=get_expression_type(return_statement.expression,current_scope).value();
            auto size=get_type_size(return_type);
            generate_expression_access(return_statement.expression,
                                       FunctionParameterIncomplete::Modifier::VALUE,
                                       CompleteType{sanema::Boolean{}},
                                       byte_code,
                                       current_scope,
                                       function_bytecode_generators,
                                       function_call_sustitutions,
                                       current_stack_address);
            // sanema::VMInstruction instruction_copy_return_address;
            // instruction_copy_return_address.opcode = OPCODE::OP_PUSH_LOCAL_SINT64;
            // instruction_copy_return_address.r_result = 0;
            // instruction_copy_return_address.registers16.r1 = current_stack_address.address;
            // byte_code.write(instruction_copy_return_address);
            sanema::VMInstruction instruction_return;
            instruction_return.r_result=0;
            instruction_return.registers16.r1=current_stack_address.address;
            instruction_return.registers16.r2=size;
            // std::cout<<"return size: "<<size<<"\n";
            instruction_return.opcode = OPCODE::OP_RETURN;
            byte_code.write(instruction_return);
          },
          [this, &total_variable_space](DeclareVariable &declare_variable) {
            auto &current_scope = scope_stack.back();
            if (current_scope.local_variables.count(declare_variable.identifier) != 0) {
              throw std::runtime_error("variable " + declare_variable.identifier + " already defined");
            }

            auto final_type=query_final_type(declare_variable.type_identifier,current_scope.types);



            if (is_user_defined(final_type)) {
              //TODO we need to call the constructor here but for now we will initialize each field with
              //their default value
                current_scope.reserve_space_for_type(final_type);

                auto user_defined_type=std::get<UserDefined>(final_type);
              //TODO urgent VariableEntry should have the final type
                current_scope.local_variables.emplace(declare_variable.identifier,
                                                   VariableEntry{declare_variable, total_variable_space});
                // byte_code.write(OPCODE::OP_RESERVE_STACK_SPACE);
                total_variable_space += get_type_size(final_type);
                // for (auto &field: user_defined_type.fields) {
                //   FunctionCall function_call;
                //   function_call.identifier = "set";
                //   function_call.arguments.emplace_back(VariableEvaluation{
                //     declare_variable.identifier + "." + field.identifier
                //   });
                //   function_call.arguments.emplace_back(get_default_literal_for_type(field.type.value()));
                //   generate_operator_call(byte_code,
                //                          function_call,
                //                          current_scope,
                //                          function_bytecode_generators,
                //                          function_call_sustitutions,current_scope.scope_address.address);
                // }

            } else {
              if (declare_variable.value.has_value()) {
                  auto value_address = current_scope.scope_address.address;
                  generate_expression_access(declare_variable.value.value(),
                                             FunctionParameterIncomplete::Modifier::VALUE,
                                             final_type,
                                             byte_code,
                                             current_scope,
                                             function_bytecode_generators,
                                             function_call_sustitutions,
                                             current_scope.scope_address);
                  current_scope.local_variables.emplace(declare_variable.identifier,
                                                     VariableEntry{declare_variable, value_address});
                  total_variable_space = current_scope.scope_address.address;
              } else {
                  current_scope.reserve_space_for_type(final_type);
                  current_scope.local_variables.emplace(declare_variable.identifier,
                                                     VariableEntry{declare_variable, total_variable_space});
                  total_variable_space += get_type_size(final_type);
              }
            }
          },
          [this](DefineFunction &function) {
            FunctionDefinitionCompleted function_definition;
            function_definition.identifier=function.identifier;
            function_definition.type=query_final_type(function.type,scope_stack.back().types);
            function_definition.body=function.body;
            for(auto parameter:function.parameters) {
              FunctionParameterCompleted function_parameter_completed;
              function_parameter_completed.identifier=parameter.identifier;
              //TODO Warning check if this work correctly
              function_parameter_completed.modifier=static_cast<FunctionParameterCompleted::Modifier>(parameter.modifier);
              function_parameter_completed.type=query_final_type(parameter.type.value(),scope_stack.back().types);
              function_definition.parameters.emplace_back(function_parameter_completed);
            }
            auto function_entry = scope_stack.back().function_collection.find_function(function_definition);
            if (!function_entry) {
              auto id = scope_stack.back().function_collection.add_function(function_definition);
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
  byte_code=ByteCode();
  pendind_to_generate_functions={};
  function_call_sustitutions={};
  scope_stack={};
  function_call_sustitutions.clear();
  scope_stack.emplace_back();
  function_bytecode_generators.map["add"] = generate_add;
  function_bytecode_generators.map["subtract"] = generate_subtract;
  function_bytecode_generators.map["multiply"] = generate_multiply;
  function_bytecode_generators.map["divide"] = generate_divide;
  function_bytecode_generators.map["set"] = generate_set;
  function_bytecode_generators.map["greater"] = generate_greater;
  function_bytecode_generators.map["less"] = generate_less;
  function_bytecode_generators.map["equal"] = generate_equal;
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
      auto final_type=function->type;
      std::int32_t parameters_size = get_type_size(final_type);
      scope_copy.reserve_space_for_type(final_type);
      for (auto &parameter: function->parameters) {
        parameter_address = parameters_size;
        if (parameter.modifier == FunctionParameterCompleted::Modifier::CONST ||
            parameter.modifier == FunctionParameterCompleted::Modifier::MUTABLE) {
          parameters_size += boost::numeric_cast<std::int64_t>(get_type_size(Integer(64)));;
          scope_copy.reserve_space_for_type(Integer(64));
        } else {
          auto parameter_final_type=parameter.type.value();
          parameters_size += boost::numeric_cast<std::int64_t>(get_type_size(parameter_final_type));
          scope_copy.reserve_space_for_type(parameter_final_type);
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
//      byte_code.code_data[caller_address].r_result = sustition.parameters_size;
    }
  }
  byte_code.function_collection=scope_stack.back().function_collection;
}
