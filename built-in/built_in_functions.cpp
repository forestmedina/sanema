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
                                          sanema::Integer(64), sanema::Float{}, sanema::Double{},sanema::String{},sanema::Boolean{}};
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
  std::vector<std::string> const function_names = {"equal", "greater", "less", "equal_greater", "equal_less"};
  std::vector<sanema::CompleteType> arithmetic_types{sanema::Integer(8), sanema::Integer(16), sanema::Integer(32),
                                                     sanema::Integer(64), sanema::Float{}, sanema::Double{}};
  for (auto const &function_name: function_names) {
    for (auto &type: arithmetic_types) {
      sanema::DefineFunction function_operation{
        function_name,
        sanema::Boolean{}, {},
        {
          sanema::FunctionParameter{"a", sanema::FunctionParameter::Modifier::VALUE, type},
          sanema::FunctionParameter{"b", sanema::FunctionParameter::Modifier::VALUE, type}
        },
      };
      function_operation.is_operator = true;
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
float mod_float(float x , float y){
  return x - y * floor(x / y);
}
std::int64_t mod_int64(std::int64_t  x, std::int64_t y){
  return x%y;
}
std::int32_t mod_int32(std::int32_t  x, std::int32_t y){
  return x%y;
}
std::int16_t mod_int16(std::int16_t  x, std::int16_t y){
  return x%y;
}
std::int8_t mod_int8(std::int8_t  x, std::int8_t y){
  return x%y;
}
float floor_float(float x ){
  return floor(x);
}
float ceil_float(float x ){
  return ceil(x);
}
float round_float(float x ){
  return round(x);
}

void add_built_in_mod(sanema::FunctionCollection &functions,sanema::BindingCollection& binding_collection){
  binding_collection.add_function_binding("mod", mod_float);
  binding_collection.add_function_binding("mod", mod_int64);
  binding_collection.add_function_binding("mod", mod_int32);
  binding_collection.add_function_binding("mod", mod_int16);
  binding_collection.add_function_binding("mod", mod_int8);
}
void add_built_in_rounding(sanema::FunctionCollection &functions, sanema::BindingCollection& binding_collection){
  binding_collection.add_function_binding("floor", floor_float);
  binding_collection.add_function_binding("ceil", ceil_float);
  binding_collection.add_function_binding("round", round_float);
}
float sqrt_float(float x ){
  return sqrt(x);
}
void add_built_in_sqrt(sanema::FunctionCollection &functions,sanema::BindingCollection& binding_collection){
  binding_collection.add_function_binding("sqrt", sqrt_float);
}
void sanema::add_built_in_functions(sanema::FunctionCollection &functions,BindingCollection& binding_collection) {
  add_built_in_arithmetic_functions(functions);
  add_built_in_boolean_functions(functions);
  add_built_in_string_functions(functions,binding_collection);
  add_built_in_return(functions,binding_collection);
  add_built_in_mod(functions,binding_collection);
  add_built_in_rounding(functions, binding_collection);
  add_built_in_sqrt(functions,binding_collection);

}