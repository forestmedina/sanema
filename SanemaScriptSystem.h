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
    void get_return_value(std::int8_t& );
    void get_return_value(std::int16_t& );
    void get_return_value(std::int32_t& );
    void get_return_value(std::int64_t& );
    void get_return_value(std::string& );
    void get_return_value(std::uint8_t& );
    void get_return_value(std::uint16_t& );
    void get_return_value(std::uint32_t& );
    void get_return_value(std::uint64_t& );
    void get_return_value(float& );
    void get_return_value(double& );
    void get_return_value(bool& );



    SanemaScriptSystem();
    virtual ~SanemaScriptSystem();

  private:
    std::unique_ptr<SanemaScriptSystemImpl> impl{};

  };
}

#endif //SANEMA_SANEMASCRIPTSYSTEM_H
