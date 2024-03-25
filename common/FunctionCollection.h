//
// Created by fores on 11/7/2023.
//

#ifndef SANEMA_FUNCTIONCOLLECTION_H
#define SANEMA_FUNCTIONCOLLECTION_H

#include <unordered_map>
#include "expressions.h"

namespace sanema {
  using FunctionID =std::uint64_t;
  struct FunctionOverloads {
    FunctionOverloads();

    std::vector<FunctionID> overloads{};
  };


  struct FunctionCollection {


    std::optional<sanema::DefineFunction> find_function(sanema::DefineFunction &function) const;

    sanema::FunctionID add_function(sanema::DefineFunction&function);
    sanema::DefineFunction*  get_function_by_id(FunctionID id);
  private:
    std::unordered_map<std::string, FunctionOverloads> functions_per_name_and_type;
    std::unordered_map<FunctionID, DefineFunction> function_collection;
    std::uint64_t next_id();

    std::uint64_t current_function_id{0};


  };


}

#endif //SANEMA_FUNCTIONCOLLECTION_H
