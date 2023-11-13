//
// Created by fores on 11/12/2023.
//

#ifndef SANEMA_HELPERS_H
#define SANEMA_HELPERS_H

#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>

template<class T>
std::optional<T> run_and_get_stack_value(std::string code) {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::stringstream stringstream(code);
  auto tokens = parser.tokenize(stringstream);
  auto block_of_code = parser.parse(tokens);
  compiler.process(block_of_code);
  sanema::VM vm;
  vm.run(compiler.byte_code);
  return vm.get_value_stack<T>();
};
#endif //SANEMA_HELPERS_H