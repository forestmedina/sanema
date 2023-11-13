//
// Created by fores on 11/7/2023.
//
#include "built_in_functions.h"

void add_built_in_arithmetic_functions(sanema::FunctionCollection &functions) {
  std::vector<std::string> function_names = {"add", "multiply", "divide", "subtract"};
  std::vector<sanema::CompleteType> arithmetic_types{sanema::Integer(8), sanema::Integer(16), sanema::Integer(32),
                                          sanema::Integer(64), sanema::Float{}, sanema::Double{}};
  for (auto &function_name: function_names) {
    functions[function_name]={};
    for (auto &type: arithmetic_types) {
      functions[function_name].overloads.emplace_back(
        sanema::DefineFunction{
          function_name,
          type, {},
          {
            sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::VALUE, type},
            sanema::FunctionParameter{"b", sanema::FunctionParameter::Modifier::VALUE, type}
          },
        });
    }
  }
  std::vector<sanema::CompleteType> primitive_types{sanema::Integer(8), sanema::Integer(16), sanema::Integer(32),
                                          sanema::Integer(64), sanema::Float{}, sanema::Double{},sanema::String{}};
  for (auto &type: primitive_types) {
    functions["set"].overloads.emplace_back(
     sanema::DefineFunction{
          "set",
          type, {},
          {
            sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::VALUE, type},
            sanema::FunctionParameter{"b", sanema::FunctionParameter::Modifier::VALUE, type}
          },
        });
  }


}

void sanema::add_built_in_functions(sanema::FunctionCollection &functions) {
  add_built_in_arithmetic_functions(functions);
}