//
// Created for sanema optimization infrastructure
//

#ifndef SANEMA_OPTIMIZER_H
#define SANEMA_OPTIMIZER_H

#include <memory>
#include <string>
#include <vector>
#include <optimizer/OptimizationPass.h>

namespace sanema {

  class Optimizer {
  public:
    void add_pass(std::unique_ptr<OptimizationPass> pass);

    void enable_pass(std::string const& name);

    void disable_pass(std::string const& name);

    void optimize(ByteCode& byte_code);

  private:
    struct PassEntry {
      std::unique_ptr<OptimizationPass> pass;
      bool enabled;
    };

    std::vector<PassEntry> passes;
  };

}

#endif //SANEMA_OPTIMIZER_H
