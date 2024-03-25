//
// Created by fores on 11/13/2023.
//

#ifndef SANEMA_SANEMASCRIPTSYSTEMIMPL_H
#define SANEMA_SANEMASCRIPTSYSTEMIMPL_H

#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <binding/BindingCollection.h>
#include <binding/FunctionBinding.h>
#include <vm/VM.h>
#include <ScriptID.h>
#include <ScriptEntry.h>
#include <interfacing/Argument.h>
namespace sanema {
  class SanemaScriptSystemImpl {
  public:
    SanemaScriptSystemImpl();

    ScriptID add_script(std::string &string);

    ScriptID add_script(std::istream &stream);

    void run_script(ScriptID id);
    void add_argument(ScriptID id,Argument const &args);
    void setup_run(ScriptID id,DefineFunction& define_function);
    void execute_run_function(sanema::ScriptID id);
    void get_return_value(std::int8_t& value);
    void get_return_value(std::int16_t& value);
    void get_return_value(std::int32_t& value);
    void get_return_value(std::int64_t& value);
    void get_return_value(std::uint8_t& value);
    void get_return_value(std::uint16_t& value);
    void get_return_value(std::uint32_t& value);
    void get_return_value(std::uint64_t& value);
    void get_return_value(std::string& value);
    void get_return_value(float& value);
    void get_return_value(double& value);
    void get_return_value(bool& value);

    BindingCollection  &get_binding_collection() ;


  private:
    SanemaParser parser;
    ByteCodeCompiler compiler;
    VM vm;

    BindingCollection binding_collection;
    FunctionCollection general_functions;
    TypeCollection external_types;


    std::unordered_map<size_t, ScriptEntry> script_collection;

    size_t next_id();
    ScriptEntry & get_script(ScriptID id);
    IPType initial_ip;


    size_t current_id = 0;
  };
}

#endif //SANEMA_SANEMASCRIPTSYSTEMIMPL_H
