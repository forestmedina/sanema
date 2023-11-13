//
// Created by fores on 11/13/2023.
//


#include  "Value.h"
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


    virtual void call(std::vector<Value> arguments) {

    };

    virtual ~FunctionPointer() = default;
  };


  template<typename FTYPE, typename ...ARGS, std::size_t... Ns>
  void callFunctionImpl(std::vector<Value> arguments, FTYPE function_pointer, std::index_sequence<Ns...>) {
    (function_pointer)((std::get<ARGS>(arguments[Ns]))...);
  }

  template<typename ...ARGS>
  struct FunctionCaller : FunctionPointer {
    using FUNC_PTR = void (*)(ARGS...);

    FunctionCaller(void(*func)(ARGS...)) {
      function_pointer = func;
    }

    virtual void call(std::vector<Value> arguments) {
      // (((TCLASS*)object)->*function_pointer)(AR);
      callFunctionImpl<FUNC_PTR, ARGS...>(arguments,
                                          function_pointer,
                                          std::index_sequence_for<ARGS...>{});
    };

    virtual ~FunctionCaller() = default;

    FUNC_PTR function_pointer;
  };
}