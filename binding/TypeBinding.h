//
// Created by fores on 11/2/2023.
//

#ifndef NATURE_TYPEBINDING_H
#define NATURE_TYPEBINDING_H
#include <string>
#include <vector>
#include "FieldBinding.h"
#include "MethodBinding.h"

namespace sanema {



  template <typename T>
  class TypeBinding {
    std::string name;
    std::vector<FieldBinding> fields;
    void construct(void * address){
      T* t_pointer=new (address) T ();
    }
    std::uint64_t size(){
      return sizeof (T);
    }
    std::vector<MethodBinding> methods;
  };
}

#endif //NATURE_TYPEBINDING_H
