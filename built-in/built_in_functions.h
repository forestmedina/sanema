//
// Created by fores on 11/7/2023.
//

#ifndef SANEMA_BUILT_IN_FUNCTIONS_H
#define SANEMA_BUILT_IN_FUNCTIONS_H
#include <unordered_map>
#include <expressions.h>
#include <parsing/FunctionCollection.h>
#include <binding/BindingCollection.h>
namespace sanema {
  void add_built_in_functions(FunctionCollection &functions,BindingCollection& binding_collection);


}

#endif //SANEMA_BUILT_IN_FUNCTIONS_H
