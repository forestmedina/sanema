//
// Created by fores on 11/13/2023.
//

#include <vm/VM.h>
#include <string>
#include <iostream>
#include <any>
#include <functional>
#include <cstdint>
#include <memory>
#include <variant>
#include <utility>

namespace sanema {


  struct FunctionPointer {


    virtual void call() {

    };
    virtual void register_function_definition(FunctionCollection &collection) {

    };

    virtual ~FunctionPointer() = default;
  };


  template<typename FTYPE, typename ...ARGS, std::size_t... Ns>
  void call_function_impl(VM vm,FTYPE function_pointer, std::index_sequence<Ns...>) {
    (function_pointer)(get_function_parameter_from_vm<ARGS>(vm,Ns)...);
  }

  template<typename FTYPE, typename RET_TYPE,typename ...ARGS, std::size_t... Ns>
  void register_function_definition_impl(std::string const&identifier ,FunctionCollection &collection,FTYPE function_pointer, std::index_sequence<Ns...>) {
    DefineFunction function;
    function.identifier=identifier;
    function.type= type_from_cpptype<RET_TYPE>();
    function.parameter.emplace_back(FunctionParameter{"",FunctionParameter::Modifier::VALUE, type_from_cpptype<ARGS>()...});
  }

  template<typename RET_TYPE, typename ...ARGS>
  struct FunctionCaller : FunctionPointer {
    using FUNC_PTR = RET_TYPE (*)(ARGS...);

    FunctionCaller(std::string const&identifier, RET_TYPE(*func)(ARGS...)):identifier(identifier) {
      function_pointer = func;
    }

     void call() override {
      call_function_impl<FUNC_PTR, ARGS...>(function_pointer,
                                          std::index_sequence_for<ARGS...>{});
    };
    void register_function_definition(FunctionCollection &collection) override{
      register_function_definition_impl<FUNC_PTR ,RET_TYPE,ARGS...>(identifier,collection,std::index_sequence_for<ARGS...>{});
    }

    virtual ~FunctionCaller() = default;

    FUNC_PTR function_pointer;
    std::string identifier;
  };
}