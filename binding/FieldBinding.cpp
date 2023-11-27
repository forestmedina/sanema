//
// Created by fores on 11/2/2023.
//

#include "FieldBinding.h"

sanema::FieldBinding::FieldBinding(std::string const &name, std::unique_ptr<FieldPointer>  field_pointer)
  : name(name), field_pointer{std::move(field_pointer)} {}
