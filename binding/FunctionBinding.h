//
// Created by fores on 11/3/2023.
//

#ifndef NATURE_FUNCTIONBINDING_H
#define NATURE_FUNCTIONBINDING_H
#include <vector>
#include "Value.h"
namespace sanema {
  struct MethodPointer {
    virtual void call(void *, std::vector<Value> const& arguments) {

    };

    virtual ~MethodPointer() = default;
  };

template<typename TCLASS,  typename FTYPE, typename ...ARGS, std::size_t... Ns>
void callMemberImpl( std::vector<Value> arguments,void* object,FTYPE function_pointer,std::index_sequence<Ns...>)
{
   (((TCLASS*)object)->*function_pointer)((std::get<ARGS>(arguments[Ns]))...);
}
template<typename TCLASS, typename ...ARGS>
struct MethodCaller:MethodPointer{
    using FUNC_PTR=void(TCLASS::*)(ARGS...);
    MethodCaller(void(TCLASS::*func)(ARGS...)){
        function_pointer=func;
    }
    virtual void call(void* object,std::vector<Value> arguments ){
       // (((TCLASS*)object)->*function_pointer)(AR);
       if(arguments.size()!=(sizeof...(ARGS))){
            std::cout<<" invalid number of parameters\n";
       }else{
       callMemberImpl<TCLASS,FUNC_PTR,ARGS...>(arguments,object,function_pointer, std::index_sequence_for<ARGS...>{} );
       }
    };
    virtual ~MethodCaller()=default;
    FUNC_PTR function_pointer;
};

  class FunctionBinding {
    std::string  name;
    std::unique_ptr<MethodPointer> method_pointer;
  };

}
#endif //NATURE_FUNCTIONBINDING_H
