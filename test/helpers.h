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
#include <SanemaScriptSystem.h>

template<class T>
std::optional<T> run_and_get_stack_value(std::string code) {
  sanema::SanemaScriptSystem sanema_script_system;
  auto script_id=sanema_script_system.add_script(code);
  sanema_script_system.run_script(script_id);
  T value{};
  sanema_script_system.get_return_value(value);
  return value;
};
template<class T, class ... ARGS>
std::optional<T> run_function_and_get_stack_value(std::string& code,std::string const& function_name,ARGS&&... args) {
  sanema::SanemaScriptSystem sanema_script_system;
  auto script_id=sanema_script_system.add_script(code);
  T value=sanema_script_system.run_function<T>(script_id,function_name,std::forward<ARGS>(args)...);
  return value;
};
#endif //SANEMA_HELPERS_H