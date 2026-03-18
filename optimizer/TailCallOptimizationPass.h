//
// Tail call optimization pass for Sanema bytecode
//

#ifndef SANEMA_TAILCALLOPTIMIZATIONPASS_H
#define SANEMA_TAILCALLOPTIMIZATIONPASS_H

#include <optimizer/OptimizationPass.h>

namespace sanema {

  class TailCallOptimizationPass : public OptimizationPass {
  public:
    void apply(ByteCode& byte_code) override;

    std::string name() const override;
  };

}

#endif //SANEMA_TAILCALLOPTIMIZATIONPASS_H
