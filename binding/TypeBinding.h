//
// Created by fores on 11/2/2023.
//

#ifndef NATURE_TYPEBINDING_H
#define NATURE_TYPEBINDING_H
#include <string>
#include <vector>
#include "FieldBinding.h"
#include "FunctionBinding.h"

namespace sanema {

  class TypeBinding {
    std::string name;
    std::string type;
    std::vector<FieldBinding> fields;
    std::vector<FunctionBinding> methods;
  };
}

#endif //NATURE_TYPEBINDING_H
