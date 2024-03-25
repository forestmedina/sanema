//
// Created by fores on 11/13/2023.
//

#ifndef SANEMA_SANEMASCRIPTSYSTEM_H
#define SANEMA_SANEMASCRIPTSYSTEM_H

#include <string>
#include <ScriptID.h>
#include <memory>
#include <binding/BindingCollection.h>
#include <interfacing/Argument.h>
namespace sanema {
  class SanemaScriptSystemImpl;
  class SanemaScriptSystem {
  public:
    ScriptID add_script(std::string  & string);
    ScriptID add_script(std::istream & stream);

    void run_script(ScriptID id);

  template <class T>
  void emplace_parameter(DefineFunction& function) {
    auto modifier=get_parameter_modifier<T>();
    FunctionParameter parameter{"",modifier,type_from_cpptype<std::remove_cvref_t<T>>()};
    function.parameters.emplace_back(parameter);
  }
    template<typename T,class ...ARGs>
    T run_function(ScriptID id,std::string const &function_name,ARGs&&... args){
      DefineFunction function;
      function.identifier=function_name;
      (emplace_parameter<ARGs>(function),...);
      setup_run(id,function);
      add_argument(id,Argument{"",std::forward<ARGs>(args)}...);
      execute_run_function(id);
      T return_value;
      get_return_value(return_value);
      return return_value;
    }
    template<class ...ARGs>
    void run_function_no_return(ScriptID id,std::string const &function_name,ARGs&&... args){
      DefineFunction function;
      function.identifier=function_name;
      (emplace_parameter<ARGs>(function),...);
      setup_run(id,function);
      add_argument(id,Argument{"",std::forward<ARGs>(args)}...);
      execute_run_function(id);
    }

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
    void add_argument(ScriptID id,Argument const &args);
    void setup_run(ScriptID id,DefineFunction& define_function);
    void execute_run_function(sanema::ScriptID id);
    BindingCollection  &get_binding_collection() ;
    std::unique_ptr<SanemaScriptSystemImpl> impl{};

  };
}

#endif //SANEMA_SANEMASCRIPTSYSTEM_H
