//
// Created by fores on 11/7/2023.
//
#include "built_in_functions.h"

#include "strings/strings.h"
#include <built-in/print.h>

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
      function_operation.is_operator=true;
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
            sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::MUTABLE, type},
            sanema::FunctionParameter{"b", sanema::FunctionParameter::Modifier::VALUE, type}
          },
        };
    function_set.is_operator=true;
    functions.add_function(function_set);
  }


}
void add_built_in_boolean_functions(sanema::FunctionCollection &functions) {
  std::vector<std::string> const function_names = {"equal", "greater", "less","equal_greater","equal_less"};
  std::vector<sanema::CompleteType> arithmetic_types{sanema::Integer(8), sanema::Integer(16), sanema::Integer(32),
                                          sanema::Integer(64), sanema::Float{}, sanema::Double{}};
  for (auto const&function_name: function_names) {
    for (auto &type: arithmetic_types) {
      sanema::DefineFunction function_operation{
          function_name,
          sanema::Boolean{}, {},
          {
            sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::VALUE, type},
            sanema::FunctionParameter{"b", sanema::FunctionParameter::Modifier::VALUE, type}
          },
        };
      function_operation.is_operator=true;
       functions.add_function(function_operation);

    }
  }
}
void add_built_in_string_functions(sanema::FunctionCollection &functions,sanema::BindingCollection& binding_collection){
  binding_collection.add_function_binding("replace_first",sanema::replace_first);
  binding_collection.add_function_binding("replace_all",sanema::replace_all);
  binding_collection.add_function_binding("concat",sanema::concat);
  binding_collection.add_function_binding("ends_with",sanema::ends_with);
  binding_collection.add_function_binding("stars_with",sanema::starts_with);
  binding_collection.add_function_binding("print",(void(*)(std::string))sanema::print);
  binding_collection.add_function_binding("print", (void(*)(std::int32_t))sanema::print);
  binding_collection.add_function_binding("print", (void(*)(std::int64_t))sanema::print);
  binding_collection.add_function_binding("print", (void(*)(float))sanema::print);

}
void add_built_in_return(sanema::FunctionCollection &functions,sanema::BindingCollection& binding_collection){
  std::vector<sanema::CompleteType> types{sanema::Integer(8), sanema::Integer(16), sanema::Integer(32),
                                          sanema::Integer(64), sanema::Float{}, sanema::Double{},sanema::Boolean{},sanema::String{}};
    for (auto &type: types) {
      sanema::DefineFunction function_operation{
          "return",
          type, {},
          {
            sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::VALUE, type}
          },
        };
       function_operation.is_operator=true;
       functions.add_function(function_operation);

    }
}
void sanema::add_built_in_functions(sanema::FunctionCollection &functions,BindingCollection& binding_collection) {
  add_built_in_arithmetic_functions(functions);
  add_built_in_boolean_functions(functions);
  add_built_in_string_functions(functions,binding_collection);
  add_built_in_return(functions,binding_collection);

}