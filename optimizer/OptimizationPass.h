//
// Created for sanema optimization infrastructure
//

#ifndef SANEMA_OPTIMIZATIONPASS_H
#define SANEMA_OPTIMIZATIONPASS_H

#include <string>
#include <vm/ByteCode.h>

namespace sanema {

  class OptimizationPass {
  public:
    virtual ~OptimizationPass() = default;

    virtual void apply(ByteCode& byte_code) = 0;

    virtual std::string name() const = 0;
  };

}

#endif //SANEMA_OPTIMIZATIONPASS_H
