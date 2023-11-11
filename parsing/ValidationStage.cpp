//
// Created by fores on 11/4/2023.
//

#include "ValidationStage.h"
#include <util/lambda_visitor.hpp>
#include <unordered_map>
#include <stdexcept>
#include <built-in/built_in_functions.h>



std::optional<sanema::DefineFunction> get_function_definition(sanema::FunctionCall &function_call,
                                            std::vector<sanema::ValidationStage::Scope> &context_frame) {
  sanema::DefineFunction function_definition;
  function_definition.identifier=function_call.identifier;
  for (auto &argument: function_call.arguments) {
    auto type= match(argument.expression,
          [&](sanema::FunctionCall &function_call_nested) -> std::optional<sanema::CompleteType> {
            auto function_definition_nested= get_function_definition(function_call_nested,
                                          context_frame);
            if(function_definition_nested.has_value()){
              return function_definition_nested.value().type;
            }else{
              return {};
            }
          },
          [&](sanema::VariableEvaluation &variable) ->std::optional<sanema::CompleteType>  {
            auto context = context_frame.back();
            if(context.variables.count(variable.identifier)>0){
              return  context.variables.at(variable.identifier).type_identifier;
            }
            return {};
          },
          [&](sanema::Literal &function_call) -> std::optional<sanema::CompleteType> {
            auto context = context_frame.back();
            return sanema::Integer{};
          }
         );
    if(!type.has_value()) return {};
    function_definition.parameter.emplace_back(sanema::FunctionParameter{"",{},type.value()});
  }
  auto found_function=find_function(context_frame.back().function_definitions,function_definition);
  return found_function;
}

void sanema::ValidationStage::process(sanema::BlockOfCode &block_of_code) {
  std::vector<Scope> scope_stack;

  scope_stack.emplace_back();
  auto& main_scope=scope_stack.back();
  add_built_in_functions(main_scope.function_definitions);
  for (auto &instruction: block_of_code.instructions) {
    match(instruction.instruction_sum,
          [&](FunctionCall &function_call) {
            auto function_definition=get_function_definition(function_call,scope_stack);
            if(function_call.identifier!="set"&&!function_definition.has_value())
              throw std::runtime_error("function "+function_call.identifier+" not found");
          },
          [&](DefineStruct &define_struct) {
            auto &scope = scope_stack.back();
            auto identifier = define_struct.user_type.value().type_id.identifier;
            if (scope.types.count(identifier) == 0) {
              scope.types[identifier] = define_struct;
            }

          },
          [&](DeclareVariable &declare_variable) {
            auto& scope=scope_stack.back();
            if(scope.variables.count(declare_variable.identifier)!=0){
              throw std::runtime_error("variable "+declare_variable.identifier+" already defined");
            }else{
              scope.variables[declare_variable.identifier]=declare_variable;
            }

          },
          [&scope_stack](DefineFunction &define_function) {
            auto define_function_copy = define_function;
            define_function_copy.body = BlockOfCode{};
            auto funcion_entry = find_function(scope_stack.back().function_definitions,
                                               define_function_copy);
            if (!funcion_entry) {
              if (scope_stack.back().function_definitions.count(define_function_copy.identifier) == 0) {
                scope_stack.back().function_definitions[define_function_copy.identifier] = {};
              }
              scope_stack.back().function_definitions[define_function_copy.identifier].overloads.emplace_back(define_function_copy);
            } else {
              throw (std::runtime_error("function already defined"));
            }
          },
          [&](BlockOfCode &) {
            scope_stack.emplace_back(scope_stack.back());
          }
         );
  }
}

sanema::FunctionOverloads::FunctionOverloads() {}
