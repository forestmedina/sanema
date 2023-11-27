//
// Created by fores on 11/14/2023.
//
#include "BindingCollection.h"

sanema::FunctionPointer &sanema::BindingCollection::get_function_by_id(std::uint64_t id) {
  return *(functions[id]);
}

void sanema::BindingCollection::register_bindings(sanema::FunctionCollection &function_collection,
                                                  sanema::TypeCollection &type_collection) {

  for (auto &function: unregistered_functions) {

    function->register_function_definition(function_collection,
                                           current_function_id);
    functions[current_function_id] = std::move(function);
    current_function_id++;
  }
  unregistered_functions.clear();
  for (auto &type: unregistered_types) {

    type->register_type_definition(type_collection,current_type_id);
    types[current_type_id] = std::move(type);
    ++current_type_id;
  }
  unregistered_types.clear();
}

sanema::TypeBindingPointer &sanema::BindingCollection::get_type_by_id(std::uint64_t id) {
  return *types[id];
}
