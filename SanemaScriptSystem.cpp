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


void sanema::SanemaScriptSystem::run_script(sanema::ScriptID id) {
  impl->run_script(id);
}

sanema::SanemaScriptSystem::~SanemaScriptSystem() {

}

sanema::SanemaScriptSystem::SanemaScriptSystem() {
  impl = std::make_unique<SanemaScriptSystemImpl>();
}

void sanema::SanemaScriptSystem::get_return_value(std::int8_t &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(std::int16_t &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(std::int32_t &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(std::int64_t &value) {
  impl->get_return_value(value);
}


void sanema::SanemaScriptSystem::get_return_value(std::uint8_t &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(std::uint16_t &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(std::uint32_t &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(std::uint64_t &value) {
  impl->get_return_value(value);
}
void sanema::SanemaScriptSystem::get_return_value(std::string &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(float &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(double &value) {
  impl->get_return_value(value);
}

void sanema::SanemaScriptSystem::get_return_value(bool &value) {
  impl->get_return_value(value);
}

sanema::BindingCollection &sanema::SanemaScriptSystem::get_binding_collection() {
  return impl->get_binding_collection();
}

void sanema::SanemaScriptSystem::add_argument(sanema::ScriptID id, const sanema::Argument &args) {
  impl->add_argument(id,args);
}

void sanema::SanemaScriptSystem::setup_run(sanema::ScriptID id, DefineFunction& define_function) {
  impl->setup_run(id,define_function);
}

void sanema::SanemaScriptSystem::execute_run_function(sanema::ScriptID id) {
  impl->execute_run_function(id);
}

void sanema::SanemaScriptSystem::replace_script(sanema::ScriptID script_id, std::string const&string) {
  impl->replace_script(script_id,string);
}

void sanema::SanemaScriptSystem::replace_script(sanema::ScriptID script_id, std::istream &stream) {
impl->replace_script(script_id,stream);
}

