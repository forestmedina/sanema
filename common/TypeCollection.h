//
// Created by fores on 11/24/2023.
//

#ifndef SANEMA_TYPECOLLECTION_H
#define SANEMA_TYPECOLLECTION_H
#include <optional>
#include <types.h>
#include <unordered_map>
#include "expressions.h"

namespace sanema {
  using TypeID =std::uint64_t;
  class TypeCollection {
  public:
    std::optional<UserDefined> find_type(CompleteType const& type );
    std::optional<UserDefined> find_type(UnidentifiedType const& type );

    sanema::TypeID add_type(UserDefined define_struct);

    UserDefined *get_type_by_id(TypeID id);
    bool containts(const std::string& identifier);

  private:
    std::unordered_map<std::string, TypeID> types_per_name_index;
    std::unordered_map<TypeID ,UserDefined> types_collection;

    std::uint64_t next_id();

    std::uint64_t current_type_id{0};


  };

}
#endif //SANEMA_TYPECOLLECTION_H
