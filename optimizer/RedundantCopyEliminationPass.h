//
// Redundant copy elimination optimization pass for Sanema bytecode
//

#ifndef SANEMA_REDUNDANTCOPYELIMINATIONPASS_H
#define SANEMA_REDUNDANTCOPYELIMINATIONPASS_H

#include <optimizer/OptimizationPass.h>

namespace sanema {

  class RedundantCopyEliminationPass : public OptimizationPass {
  public:
    void apply(ByteCode& byte_code) override;

    std::string name() const override;

  private:
    bool eliminate_set_push_pairs(ByteCode& byte_code);
    bool eliminate_push_set_push_chains(ByteCode& byte_code);
    bool eliminate_return_double_copy(ByteCode& byte_code);
  };

}

#endif //SANEMA_REDUNDANTCOPYELIMINATIONPASS_H
