//
// Created by fores on 11/7/2023.
//
#include "built_in_functions.h"


void add_built_in_arithmetic_functions(sanema::FunctionCollection &functions) {
  std::vector<std::string> const function_names = {"add", "multiply", "divide", "subtract"};
  std::vector<sanema::CompleteType> arithmetic_types{sanema::Integer(8), sanema::Integer(16), sanema::Integer(32),
                                          sanema::Integer(64), sanema::Float{}, sanema::Double{}};
  for (auto const&function_name: function_names) {
    for (auto &type: arithmetic_types) {
      sanema::DefineFunction function_operation{
          function_name,
          type, {},
          {
            sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::VALUE, type},
            sanema::FunctionParameter{"b", sanema::FunctionParameter::Modifier::VALUE, type}
          },
        };
       functions.add_function(function_operation);

    }
  }
  std::vector<sanema::CompleteType> const primitive_types{sanema::Integer(8), sanema::Integer(16), sanema::Integer(32),
                                          sanema::Integer(64), sanema::Float{}, sanema::Double{},sanema::String{}};
  for (auto &type: primitive_types) {
    sanema::DefineFunction function_set{
          "set",
          type, {},
          {
            sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::VALUE, type},
            sanema::FunctionParameter{"b", sanema::FunctionParameter::Modifier::VALUE, type}
          },
        };
    functions.add_function(function_set);
  }


}

void sanema::add_built_in_functions(sanema::FunctionCollection &functions) {
  add_built_in_arithmetic_functions(functions);
}