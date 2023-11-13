//
// Created by fores on 11/7/2023.
//

#include "FunctionCollection.h"
#include <algorithm>
std::optional<sanema::DefineFunction> sanema::find_function(sanema::FunctionCollection& collection , sanema::DefineFunction &function) {


  if (collection.count(function.identifier) == 0) {
    return {};
  }
  auto &function_overloads = collection.at(function.identifier);
  auto function_found_iter = std::find_if(function_overloads.overloads.begin(),
                                       function_overloads.overloads.end(),
  [&function](auto& curren_function){return  function.is_compatible(curren_function);});
  if (function_found_iter != function_overloads.overloads.end()) {
    return *function_found_iter;
  }
  return {};
}

void sanema::add_function(sanema::FunctionCollection &collection, sanema::DefineFunction &function) {
    if(collection.count(function.identifier)==0) collection[function.identifier]=FunctionOverloads{};
    collection[function.identifier].overloads.emplace_back(function);
}
