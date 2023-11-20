//
// Created by fores on 11/13/2023.
//

#include "SanemaScriptSystemImpl.h"
#include <sstream>
#include <built-in/strings/strings.h>
#include <binding/FunctionBinding.h>
#include <built-in/built_in_functions.h>
sanema::ScriptID sanema::SanemaScriptSystemImpl::add_script(std::string &string) {
  std::stringstream string_stream(string);
  return add_script(string_stream);
}

sanema::ScriptID sanema::SanemaScriptSystemImpl::add_script(std::istream &stream) {
  auto tokens =parser.tokenize(stream);
  auto block_of_code=parser.parse(tokens);
  compiler.process(block_of_code,general_functions);
  auto id=ScriptID{next_id()};
  script_collection[id.id]=ScriptEntry{id,std::move(compiler.byte_code)};
  return id;
}

void sanema::SanemaScriptSystemImpl::run_script(sanema::ScriptID id) {
  auto& script=get_script(id);
  vm.run(script.bytecode,binding_collection);
}

size_t sanema::SanemaScriptSystemImpl::next_id() {
      ++current_id;
      return current_id;
}

sanema::ScriptEntry &sanema::SanemaScriptSystemImpl::get_script(ScriptID id) {
  return script_collection.at(id.id);
}

sanema::SanemaScriptSystemImpl::SanemaScriptSystemImpl(): binding_collection{} {
  add_built_in_functions(general_functions,binding_collection);
  binding_collection.register_bindings(general_functions);

}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int8_t &value) {
  value=vm.get_value_stack<std::int8_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int16_t &value) {
  value=vm.get_value_stack<std::int16_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int32_t &value) {
  value=vm.get_value_stack<std::int32_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int64_t &value) {
  value=vm.get_value_stack<std::int64_t>().value_or(value);
}


void sanema::SanemaScriptSystemImpl::get_return_value(std::uint8_t &value) {
  value=vm.get_value_stack<std::uint8_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::uint16_t &value) {
  value=vm.get_value_stack<std::uint16_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::uint32_t &value) {
  value=vm.get_value_stack<std::uint32_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::uint64_t &value) {
  value=vm.get_value_stack<std::uint64_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::string &value) {
  auto reference=vm.get_value_stack<StringReference>();
  if(reference.has_value()){
    value=vm.get_string(reference.value());
  }else{
    value="";
  }

}

void sanema::SanemaScriptSystemImpl::get_return_value(float &value) {
  value=vm.get_value_stack<float>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(double &value) {
  value=vm.get_value_stack<double>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(bool &value) {
  value=vm.get_value_stack<bool>().value_or(value);
}
