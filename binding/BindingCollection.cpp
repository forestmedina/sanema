//
// Created by fores on 11/14/2023.
//
#include "BindingCollection.h"

sanema::FunctionPointer& sanema::BindingCollection::get_function_by_id(std::uint64_t id) {
  return *(function_pointers[id]);
}

void sanema::BindingCollection::register_bindings(sanema::FunctionCollection &collection) {
  int id=0;
    for(auto const& function:function_pointers){
      function->register_function_definition(collection,id);
      id++;
    }
}
