//
// Created by fores on 11/13/2023.
//

#ifndef SANEMA_SANEMASCRIPTSYSTEM_H
#define SANEMA_SANEMASCRIPTSYSTEM_H

#include <string>
#include <ScriptID.h>
#include <memory>
#include <binding/BindingCollection.h>
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


    template<class F>
      void add_function(std::string const &identifier, F f){
        get_binding_collection().add_function_binding(identifier,f);
      }
      template<class T>
      TypeBindingPointer* add_type(std::string const &identifier){
        return get_binding_collection().add_type_binding<T>(identifier);
      }
    SanemaScriptSystem();
    virtual ~SanemaScriptSystem();

  private:
    BindingCollection  &get_binding_collection() ;
    std::unique_ptr<SanemaScriptSystemImpl> impl{};

  };
}

#endif //SANEMA_SANEMASCRIPTSYSTEM_H
