//
// Created by fores on 11/12/2023.
//

#ifndef SANEMA_HELPERS_H
#define SANEMA_HELPERS_H

#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <binding/BindingCollection.h>
#include <vm/VM.h>
#include <sstream>
#include <built-in/built_in_functions.h>

template<class T>
std::optional<T> run_and_get_stack_value(std::string code) {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::stringstream stringstream(code);
  auto tokens = parser.tokenize(stringstream);
  auto block_of_code = parser.parse(tokens);
  sanema::FunctionCollection  built_in_functions;
  sanema::add_built_in_functions(built_in_functions);
  sanema::BindingCollection binding_collection;
  compiler.process(block_of_code,built_in_functions);

  sanema::VM vm{};
  vm.run(compiler.byte_code,binding_collection);
  return vm.get_value_stack<T>();
};
#endif //SANEMA_HELPERS_H