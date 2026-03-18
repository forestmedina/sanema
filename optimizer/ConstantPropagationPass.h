//
// Constant propagation optimization pass for Sanema bytecode
//

#ifndef SANEMA_CONSTANTPROPAGATIONPASS_H
#define SANEMA_CONSTANTPROPAGATIONPASS_H

#include <optimizer/OptimizationPass.h>

namespace sanema {

  class ConstantPropagationPass : public OptimizationPass {
  public:
    void apply(ByteCode& byte_code) override;

    std::string name() const override;

  private:
    bool propagate_once(ByteCode& byte_code);
  };

}

#endif //SANEMA_CONSTANTPROPAGATIONPASS_H
