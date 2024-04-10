//
// Created by fores on 11/13/2023.
//

#include "SanemaScriptSystem.h"
#include "SanemaScriptSystemImpl.h"

sanema::ScriptID sanema::SanemaScriptSystem::add_script(std::istream &stream) {
  return impl->add_script(stream);
}

sanema::ScriptID sanema::SanemaScriptSystem::add_script(std::string const &string) {
  return impl->add_script(string);
}


void sanema::SanemaScriptSystem::run_script(sanema::ScriptID id,std::uint32_t vm_index) {
  impl->run_script(id, vm_index);
}

sanema::SanemaScriptSystem::~SanemaScriptSystem() {

}

sanema::SanemaScriptSystem::SanemaScriptSystem(unsigned int number_of_vms,unsigned int mb_per_vm) {
  impl = std::make_unique<SanemaScriptSystemImpl>(number_of_vms, mb_per_vm);
}

void sanema::SanemaScriptSystem::get_return_value(std::int8_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(std::int16_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(std::int32_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(std::int64_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}


void sanema::SanemaScriptSystem::get_return_value(std::uint8_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(std::uint16_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(std::uint32_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(std::uint64_t &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}
void sanema::SanemaScriptSystem::get_return_value(std::string &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(float &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(double &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

void sanema::SanemaScriptSystem::get_return_value(bool &value,std::uint32_t vm_index) {
  impl->get_return_value(value, vm_index);
}

sanema::BindingCollection &sanema::SanemaScriptSystem::get_binding_collection() {
  return impl->get_binding_collection();
}

void sanema::SanemaScriptSystem::add_argument(sanema::ScriptID id, const sanema::Argument &args,std::uint32_t vm_index) {
  impl->add_argument(id, args, vm_index);
}

void sanema::SanemaScriptSystem::setup_run(sanema::ScriptID id, FunctionID  function_id,std::uint32_t vm_index) {
  impl->setup_run(id, function_id, vm_index);
}

void sanema::SanemaScriptSystem::execute_run_function(sanema::ScriptID id, std::uint32_t vm_index) {
  impl->execute_run_function(id, vm_index);
}

void sanema::SanemaScriptSystem::replace_script(sanema::ScriptID script_id, std::string const&string) {
  impl->replace_script(script_id,string);
}

void sanema::SanemaScriptSystem::replace_script(sanema::ScriptID script_id, std::istream &stream) {
impl->replace_script(script_id,stream);
}

std::optional<sanema::FunctionID> sanema::SanemaScriptSystem::get_function_id(ScriptID id,sanema::FunctionDefinitionCompleted &define_function) {
  return impl->get_function_id(id,define_function);
}

