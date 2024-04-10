//
// Created by fores on 11/2/2023.
//

#include "FieldBinding.h"

sanema::FieldBinding::FieldBinding(std::string const &name, size_t  offset,std::unique_ptr<FieldType> field_type)
  : name(name), offset{offset},field_type{std::move(field_type)} {}
