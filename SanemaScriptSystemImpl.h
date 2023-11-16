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

namespace sanema {
  class SanemaScriptSystemImpl {
  public:
    SanemaScriptSystemImpl();

    ScriptID add_script(std::string &string);

    ScriptID add_script(std::istream &stream);

    void run_script(ScriptID id);
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

  private:
    SanemaParser parser;
    ByteCodeCompiler compiler;
    VM vm;

    BindingCollection binding_collection;
    FunctionCollection general_functions;

    std::unordered_map<size_t, ScriptEntry> script_collection;

    size_t next_id();
    ScriptEntry & get_script(ScriptID id);



    size_t current_id = 0;
  };
}

#endif //SANEMA_SANEMASCRIPTSYSTEMIMPL_H
