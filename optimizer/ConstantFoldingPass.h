//
// Constant folding optimization pass for Sanema bytecode
//

#ifndef SANEMA_CONSTANTFOLDINGPASS_H
#define SANEMA_CONSTANTFOLDINGPASS_H

#include <optimizer/OptimizationPass.h>

namespace sanema {

  class ConstantFoldingPass : public OptimizationPass {
  public:
    void apply(ByteCode& byte_code) override;

    std::string name() const override;

  private:
    bool fold_once(ByteCode& byte_code);
  };

}

#endif //SANEMA_CONSTANTFOLDINGPASS_H
