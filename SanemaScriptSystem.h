//
// Created by fores on 11/13/2023.
//

#ifndef SANEMA_SANEMASCRIPTSYSTEM_H
#define SANEMA_SANEMASCRIPTSYSTEM_H

#include <string>
#include <ScriptID.h>
#include <memory>
namespace sanema {
  class SanemaScriptSystemImpl;
  class SanemaScriptSystem {
  public:
    ScriptID add_script(std::string  & string);
    ScriptID add_script(std::istream & stream);

    void run_script(ScriptID id);

    SanemaScriptSystem();

    virtual ~SanemaScriptSystem();

  private:
    std::unique_ptr<SanemaScriptSystemImpl> impl{};

  };
}

#endif //SANEMA_SANEMASCRIPTSYSTEM_H
