//
// Created by fores on 11/13/2023.
//

#include "FunctionBinding.h"
namespace sanema{
  class BindingCollection{
  public :
    template<typename ...ARGS>
    void  add_function(void(*func)(ARGS...)){
        function_pointers.emplace_back(std::make_unique<FunctionCaller>(std::forward<ARGS>()...));
    }
  private:
    std::vector<std::unique_ptr<FunctionPointer>> function_pointers;
  };
}