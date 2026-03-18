//
// Dead code elimination optimization pass for Sanema bytecode
//

#ifndef SANEMA_DEADCODEELIMINATIONPASS_H
#define SANEMA_DEADCODEELIMINATIONPASS_H

#include <optimizer/OptimizationPass.h>

namespace sanema {

  class DeadCodeEliminationPass : public OptimizationPass {
  public:
    void apply(ByteCode& byte_code) override;

    std::string name() const override;

  private:
    void eliminate_unreachable_code(ByteCode& byte_code);
    void eliminate_dead_stores(ByteCode& byte_code);
  };

}

#endif //SANEMA_DEADCODEELIMINATIONPASS_H
