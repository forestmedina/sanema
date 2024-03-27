//
// Created by fores on 11/7/2023.
//

#include "FunctionCollection.h"
#include <algorithm>

std::optional<sanema::DefineFunction> sanema::FunctionCollection::find_function(sanema::DefineFunction&function) const{
  if (functions_per_name_and_type.count(function.identifier) == 0) {
    return {};
  }
  auto&function_overloads = functions_per_name_and_type.at(function.identifier);
  auto function_found_iter = std::find_if(function_overloads.overloads.begin(),
                                          function_overloads.overloads.end(),
                                          [&function,this](auto&function_id) {
                                            auto current_function = function_collection.at(function_id);
                                            return function.is_compatible(current_function);
                                          });
  if (function_found_iter != function_overloads.overloads.end()) {
    return function_collection.at(*function_found_iter);
  }
  return {};
}

sanema::FunctionID sanema::FunctionCollection::add_function(sanema::DefineFunction&function) {
  if (functions_per_name_and_type.count(function.identifier) == 0) {
    functions_per_name_and_type[function.identifier] = FunctionOverloads{};
  }
  auto id = next_id();
  function.id = id;
  functions_per_name_and_type[function.identifier].overloads.emplace_back(id);
  function_collection[id] = function;
  return id;
}

std::uint64_t sanema::FunctionCollection::next_id() {
  current_function_id++;
  return current_function_id;
}

sanema::DefineFunction const* sanema::FunctionCollection::get_function_by_id(sanema::FunctionID id) const {
  if (function_collection.contains(id)) {
    return &function_collection.at(id);
  }
  return nullptr;
}
sanema::DefineFunction* sanema::FunctionCollection::get_function_by_id(sanema::FunctionID id)  {
  if (function_collection.contains(id)) {
    return &function_collection.at(id);
  }
  return nullptr;
}
