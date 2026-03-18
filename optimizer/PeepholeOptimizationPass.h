//
// Peephole optimization pass for Sanema bytecode
//

#ifndef SANEMA_PEEPHOLEOPTIMIZATIONPASS_H
#define SANEMA_PEEPHOLEOPTIMIZATIONPASS_H

#include <optimizer/OptimizationPass.h>

namespace sanema {

  class PeepholeOptimizationPass : public OptimizationPass {
  public:
    void apply(ByteCode& byte_code) override;

    std::string name() const override;

  private:
    bool optimize_once(ByteCode& byte_code);
  };

}

#endif //SANEMA_PEEPHOLEOPTIMIZATIONPASS_H
