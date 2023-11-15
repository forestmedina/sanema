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
