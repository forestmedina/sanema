//
// Created by fores on 11/3/2023.
//

#ifndef NATURE_BACKEND_H
#define NATURE_BACKEND_H
#include "../expressions.h"
#include "common/FunctionCollection.h"
namespace sanema {
  class Backend {
  public:
    virtual void process(BlockOfCode &block_of_code,FunctionCollection& built_in_functions)=0;
  };

}


#endif //NATURE_BACKEND_H
