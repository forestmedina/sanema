//
// Created by fores on 11/13/2023.
//

#include "SanemaScriptSystem.h"
#include "SanemaScriptSystemImpl.h"

sanema::ScriptID sanema::SanemaScriptSystem::add_script(std::istream &stream) {
  return impl->add_script(stream);
}

sanema::ScriptID  sanema::SanemaScriptSystem::add_script(std::string &string) {
  return impl->add_script(string);
}


void sanema::SanemaScriptSystem::run_script(sanema::ScriptID id) {
      impl->run_script(id);
}

sanema::SanemaScriptSystem::~SanemaScriptSystem() {

}

sanema::SanemaScriptSystem::SanemaScriptSystem() {
  impl=std::make_unique<SanemaScriptSystemImpl>();
}

