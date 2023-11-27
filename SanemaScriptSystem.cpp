//
// Created by fores on 11/13/2023.
//

#include "SanemaScriptSystem.h"
#include "SanemaScriptSystemImpl.h"

sanema::ScriptID sanema::SanemaScriptSystem::add_script(std::istream &stream) {
  return impl->add_script(stream);
}

sanema::ScriptID sanema::SanemaScriptSystem::add_script(std::string &string) {
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

