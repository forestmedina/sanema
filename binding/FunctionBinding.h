//
// Created by fores on 11/13/2023.
//
#ifndef SANEMA_FUNCTION_POINTER
#define SANEMA_FUNCTION_POINTER
#include <vm/VM.h>
#include <string>
#include <iostream>
#include <any>
#include <functional>
#include <cstdint>
#include <memory>
#include <utility>
#include <variant>
#include <utility>
#include "common/FunctionCollection.h"
namespace sanema {


  struct FunctionPointer {


    virtual void call(VM& vm)=0;
    virtual void register_function_definition(FunctionCollection &collection,std::uint64_t id)=0;

    virtual ~FunctionPointer() = default;
  };

  template <typename T>
  FunctionParameter::Modifier get_parameter_modifier(){
     auto modifier=std::is_reference_v<T>? (std::is_const_v<std::remove_reference_t<T>>?FunctionParameter::Modifier::CONST:FunctionParameter::Modifier::MUTABLE ):FunctionParameter::Modifier::VALUE;
    return modifier;
  }
  template<typename RET_TYPE,typename FTYPE, typename ...ARGS, std::size_t... Ns>
  std::enable_if_t<std::negation_v<std::is_void<RET_TYPE>>>
    call_function_impl(VM& vm,FTYPE function_pointer, std::index_sequence<Ns...> is) {
      vm.prepare_function_parameters(is.size());
      auto return_value=(function_pointer)(get_function_parameter_from_vm<ARGS>(vm,Ns,get_parameter_modifier<ARGS>())...);
      push_function_return_to_vm(vm,return_value);

  }
  template<typename RET_TYPE,typename FTYPE, typename ...ARGS, std::size_t... Ns>
  std::enable_if_t<std::is_void_v<RET_TYPE>> call_function_impl(VM& vm,FTYPE function_pointer, std::index_sequence<Ns...> is) {
    vm.prepare_function_parameters(is.size());
    (function_pointer)(get_function_parameter_from_vm<ARGS>(vm,Ns,get_parameter_modifier<ARGS>())...);
  }


template <class T>
  void emplace_parameter(DefineFunction& function) {
    auto modifier=get_parameter_modifier<T>();
    FunctionParameter parameter{"",modifier,type_from_cpptype<std::remove_cvref_t<T>>()};
    function.parameters.emplace_back(parameter);
  }
  template<typename FTYPE, typename RET_TYPE,typename ...ARGS, std::size_t... Ns>
  void register_function_definition_impl(std::string const& identifier,std::uint64_t id,FunctionCollection &collection,FTYPE function_pointer, std::index_sequence<Ns...>) {
    DefineFunction function;
    function.identifier=identifier;
    auto value=type_from_cpptype<std::remove_cvref_t<RET_TYPE>>();
    function.type= value;
    function.external_id=id;
    (emplace_parameter<ARGS>(function),...);
    collection.add_function(function);

  }

  template<typename RET_TYPE, typename ...ARGS>
  struct FunctionCaller : public FunctionPointer {
    using FUNC_PTR = RET_TYPE (*)(ARGS...);

    FunctionCaller( std::string   identifier,RET_TYPE(*func)(ARGS...)):identifier(std::move(identifier)) {
      function_pointer = func;
    }

     void call(VM& vm) override {
      call_function_impl<RET_TYPE,FUNC_PTR, ARGS...>(vm,function_pointer,
                                          std::index_sequence_for<ARGS...>{});
    };
    void register_function_definition(FunctionCollection &collection,std::uint64_t id) override{
      register_function_definition_impl<FUNC_PTR ,RET_TYPE,ARGS...>(identifier,id,collection,function_pointer,std::index_sequence_for<ARGS...>{});
    }

     ~FunctionCaller() override = default;

    FUNC_PTR function_pointer;
    std::string identifier;
  };

}
#endif