//
// Created by fores on 11/13/2023.
//

#include "SanemaScriptSystemImpl.h"
#include <sstream>
#include <built-in/strings/strings.h>
#include <binding/FunctionBinding.h>
#include <built-in/built_in_functions.h>

sanema::ScriptID sanema::SanemaScriptSystemImpl::add_script(std::string const &string) {
  std::stringstream string_stream(string);
  return add_script(string_stream);
}

sanema::ScriptID sanema::SanemaScriptSystemImpl::add_script(std::istream &stream) {
  auto id = ScriptID{next_id()};
  replace_script(id,stream);
  return id;
}

void sanema::SanemaScriptSystemImpl::run_script(sanema::ScriptID id, std::uint32_t vm_index) {
auto& vm=vms.at(vm_index);
  auto &script = get_script(id);
  vm.run(script.bytecode,
         binding_collection);
}

size_t sanema::SanemaScriptSystemImpl::next_id() {
  ++current_id;
  return current_id;
}

sanema::ScriptEntry &sanema::SanemaScriptSystemImpl::get_script(ScriptID id) {
  return script_collection.at(id.id);
}

sanema::SanemaScriptSystemImpl::SanemaScriptSystemImpl(unsigned int number_of_vms,unsigned int mb_per_vm) : binding_collection{},vms{} {
  add_built_in_functions(general_functions,
                         binding_collection);
  binding_collection.register_bindings(general_functions,external_types);
  for(int i=0;i<number_of_vms;++i) {
    vms.emplace_back(mb_per_vm);
  }

}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int8_t &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::int8_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int16_t &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::int16_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int32_t &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::int32_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::int64_t &value, unsigned int vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::int64_t>().value_or(value);
}


void sanema::SanemaScriptSystemImpl::get_return_value(std::uint8_t &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::uint8_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::uint16_t &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::uint16_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::uint32_t &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::uint32_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::uint64_t &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<std::uint64_t>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(std::string &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  auto reference = vm.get_value_stack<StringReference>();
//  std::cout <<"getting string : "<<reference.value().ref<<" location: "<<(reference.value().location==StringLocation::LiteralPool?"literal":"stack")<<"\n";
  if (reference.has_value()) {
    value = vm.get_string(reference.value());
  } else {
    value = "";
  }
}

void sanema::SanemaScriptSystemImpl::get_return_value(float &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<float>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(double &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<double>().value_or(value);
}

void sanema::SanemaScriptSystemImpl::get_return_value(bool &value, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  value = vm.get_value_stack<bool>().value_or(value);
}

void* sanema::SanemaScriptSystemImpl::get_return_pointer(std::uint32_t vm_index) {
  return vms[vm_index].get_stack_pointer();
}

sanema::BindingCollection &sanema::SanemaScriptSystemImpl::get_binding_collection() {
  return binding_collection;
}

void sanema::SanemaScriptSystemImpl::add_argument(
  sanema::ScriptID id, const sanema::Argument &args, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
    vm.add_external_argument(args);
}

void sanema::SanemaScriptSystemImpl::execute_run_function(sanema::ScriptID id, std::uint32_t vm_index) {
  auto& vm=vms.at(vm_index);
  auto &script = get_script(id);
  vm.run(script.bytecode,binding_collection,initial_ip);
}

void sanema::SanemaScriptSystemImpl::setup_run(sanema::ScriptID id, FunctionID &fuction_id, std::uint32_t vm_index) {
  auto &script = get_script(id);
  auto& vm=vms.at(vm_index);
  initial_ip= vm.setup_run(script.bytecode, binding_collection,fuction_id);
}

void sanema::SanemaScriptSystemImpl::replace_script(sanema::ScriptID id, std::string const&string) {
  std::stringstream string_stream(string);
  replace_script(id,string_stream);
}

void sanema::SanemaScriptSystemImpl::replace_script(sanema::ScriptID id, std::istream &stream) {
  binding_collection.register_bindings(general_functions,external_types);
  auto tokens = parser.tokenize(stream);
  auto block_of_code = parser.parse(tokens);
  compiler.process(block_of_code,
                   general_functions,
                   external_types);
  std::cout << "BYTECODE BEGIN*******************************\n\n";
  compiler.byte_code.print();
  std::cout << "\n\nBYTECODE END*******************************\n\n";
  std::cout<<std::flush;

  script_collection[id.id] = ScriptEntry{id, std::move(compiler.byte_code)};
}

std::optional<sanema::FunctionID> sanema::SanemaScriptSystemImpl::get_function_id(ScriptID id,sanema::FunctionDefinitionCompleted &define_function) {
  auto function = get_script(id).bytecode.function_collection.find_function(define_function);
  if (function.has_value()) {
    return function->id;
  }
  return std::nullopt;
}