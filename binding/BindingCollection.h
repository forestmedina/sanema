//
// Created by fores on 11/13/2023.
//

#include "FunctionBinding.h"
namespace sanema{
  class BindingCollection{
  public :
    template<typename T>
    void  add_function(std::string const& identifier,T  t){
        FunctionCaller function_caller{identifier,t}
        function_pointers.emplace_back(std::make_unique<FunctionCaller>(identifier,t));
    }
  private:
    std::vector<std::unique_ptr<FunctionPointer>> function_pointers;
  };
}