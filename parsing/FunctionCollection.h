//
// Created by fores on 11/7/2023.
//

#ifndef SANEMA_FUNCTIONCOLLECTION_H
#define SANEMA_FUNCTIONCOLLECTION_H

#include <unordered_map>
#include <expressions.h>

namespace sanema {
  struct FunctionOverloads {
    FunctionOverloads();

    std::vector<DefineFunction> overloads{};
  };

  using FunctionCollection = std::unordered_map<std::string, FunctionOverloads>;

  std::optional<sanema::DefineFunction>
  find_function(sanema::FunctionCollection &collection, sanema::DefineFunction &function);

  void add_function(sanema::FunctionCollection &collection, sanema::DefineFunction &function);
}

#endif //SANEMA_FUNCTIONCOLLECTION_H
