//
// Created by fores on 11/13/2023.
//
#ifndef SANEMA_BINDING_COLLECTION
#define SANEMA_BINDING_COLLECTION

#include "FunctionBinding.h"
#include "common/FunctionCollection.h"
#include "common/TypeCollection.h"
#include "TypeBinding.h"
#include <memory>

namespace sanema {

  class BindingCollection {
  public :
    template<typename F>
    void add_function_binding(std::string const &identifier, F f) {
      FunctionCaller function_caller{identifier, f};
      unregistered_functions.emplace_back(std::make_unique<decltype(function_caller)>(function_caller));
    }

    template<typename T>
    TypeBindingPointer *add_type_binding(std::string const &identifier) {
      TypeBinding<T> type_binding{identifier};
      unregistered_types.emplace_back(std::make_unique<decltype(type_binding)>(std::move(type_binding)));
      return unregistered_types.back().get();
    }

    FunctionPointer &get_function_by_id(std::uint64_t id);
    TypeBindingPointer &get_type_by_id( std::uint64_t id);

    void register_bindings(FunctionCollection &function_collection, TypeCollection& type_collection);

  private:
    std::vector<std::unique_ptr<sanema::FunctionPointer>> unregistered_functions;
    std::vector<std::unique_ptr<sanema::TypeBindingPointer>> unregistered_types;
    std::map<uint64_t,std::unique_ptr<sanema::FunctionPointer>> functions;
    std::map<uint64_t,std::unique_ptr<sanema::TypeBindingPointer>> types;
    int current_function_id=0;
    int current_type_id=0;
  };

}
#endif