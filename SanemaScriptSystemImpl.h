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
#include <common/FunctionCollection.h>
namespace sanema {
  class SanemaScriptSystemImpl {
  public:
    SanemaScriptSystemImpl(unsigned int number_of_vms,unsigned int mb_per_vm);

    ScriptID add_script(std::string const&string);
    void replace_script(ScriptID id,std::string const&string);
    void replace_script(ScriptID id,std::istream &stream);
    ScriptID add_script(std::istream &stream);

    void run_script(sanema::ScriptID id, std::uint32_t vm_index);
    void add_argument(sanema::ScriptID id, const sanema::Argument &args, std::uint32_t vm_index);
    void setup_run(sanema::ScriptID id, FunctionID &define_function, std::uint32_t vm_index);
    std::optional<FunctionID> get_function_id(ScriptID id, FunctionDefinitionCompleted& define_function);
    void execute_run_function(sanema::ScriptID id, std::uint32_t vm_index);
    template <typename  T>
    void get_return_native_type(T &value, std::uint32_t vm_index) {
      auto& vm=vms.at(vm_index);
      value = vm.get_value_stack<T>().value_or(value);
    };
    void get_return_value(std::int8_t &value, std::uint32_t vm_index);
    void get_return_value(std::int16_t &value, std::uint32_t vm_index);
    void get_return_value(std::int32_t &value, std::uint32_t vm_index);
    void get_return_value(std::int64_t &value, unsigned int vm_index);
    void get_return_value(std::uint8_t &value, std::uint32_t vm_index);
    void get_return_value(std::uint16_t &value, std::uint32_t vm_index);
    void get_return_value(std::uint32_t &value, std::uint32_t vm_index);
    void get_return_value(std::uint64_t &value, std::uint32_t vm_index);
    void get_return_value(std::string &value, std::uint32_t vm_index);
    void get_return_value(float &value, std::uint32_t vm_index);
    void get_return_value(double &value, std::uint32_t vm_index);
    void get_return_value(bool &value, std::uint32_t vm_index);
    void* get_return_pointer( std::uint32_t vm_index);

    BindingCollection  &get_binding_collection() ;


  private:
    SanemaParser parser;
    ByteCodeCompiler compiler;
    std::vector<VM> vms;

    BindingCollection binding_collection;
    FunctionCollection general_functions;
    TypeCollection external_types;


    std::unordered_map<size_t, ScriptEntry> script_collection;

    size_t next_id();
    ScriptEntry & get_script(ScriptID id);
    IPType initial_ip;


    size_t current_id = 0;
  };

  // template<typename T>
  // void SanemaScriptSystemImpl::get_return_native_type(T& value, std::uint32_t vm_index) {
  // }
}

#endif //SANEMA_SANEMASCRIPTSYSTEMIMPL_H
