//
// Created for Epic #31: Yieldable Native Functions in Sanema
//
#ifndef SANEMA_IYIELDABLE_FUNCTION_H
#define SANEMA_IYIELDABLE_FUNCTION_H

namespace sanema {
  class VM;

  struct IYieldableFunction {
    virtual void tick()            = 0;  // called each frame until finished
    virtual bool finished()        = 0;  // true when the action is done
    virtual void write_return(VM &vm) = 0; // write return value to operand stack
    virtual ~IYieldableFunction()  = default;
  };
}

#endif //SANEMA_IYIELDABLE_FUNCTION_H
