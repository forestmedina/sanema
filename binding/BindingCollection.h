//
// Created by fores on 11/13/2023.
//
#ifndef SANEMA_BINDING_COLLECTION
#define SANEMA_BINDING_COLLECTION

#include "FunctionBinding.h"
#include "parsing/FunctionCollection.h"
#include <memory>

namespace sanema {

  class BindingCollection {
  public :
    template<typename F>
    void add_function_binding(std::string const &identifier, F f) {
      FunctionCaller function_caller{identifier, f};
      function_pointers.emplace_back(std::make_unique<decltype(function_caller)>(function_caller));
    }

    FunctionPointer &get_function_by_id(std::uint64_t id);

    void register_bindings(FunctionCollection &collection);

  private:
    std::vector<std::unique_ptr<FunctionPointer>> function_pointers;
  };

}
#endif