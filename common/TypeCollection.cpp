//
// Created by fores on 11/24/2023.
//
#include <util/lambda_visitor.hpp>
#include "TypeCollection.h"

std::optional<sanema::UserDefined> sanema::TypeCollection::find_type(const sanema::CompleteType &type) {
  return match(type,
        [this](UserDefined const & user_defined) -> std::optional<sanema::UserDefined>{
          if(types_per_name_index.contains(user_defined.type_id.identifier)){
            return types_collection.at(types_per_name_index.at(user_defined.type_id.identifier));
          }
          return {};
        },
        [](auto& ignore)->std::optional<sanema::UserDefined>{
          return {};
         }
       );
}

sanema::TypeID sanema::TypeCollection::add_type(sanema::UserDefined define_struct) {
  auto id=next_id();
  types_per_name_index[define_struct.type_id.identifier]=id;
  types_collection.try_emplace(id,define_struct);
  return id;
}

sanema::UserDefined *sanema::TypeCollection::get_type_by_id(sanema::TypeID id) {
  if( types_collection.contains(id)){
    return &types_collection.at(id);
  };
  return nullptr;
}

std::uint64_t sanema::TypeCollection::next_id() {
  current_type_id++;
  return current_type_id;
}

bool sanema::TypeCollection::containts(const std::string& identifier) {
  return types_per_name_index.contains(identifier);
}
