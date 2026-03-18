//
// Created for sanema optimization infrastructure
//

#include "Optimizer.h"
#include <iostream>
#include <print>

namespace sanema {

  void Optimizer::add_pass(std::unique_ptr<OptimizationPass> pass) {
    passes.push_back(PassEntry{std::move(pass), false});
  }

  void Optimizer::enable_pass(std::string const& name) {
    for (auto& entry : passes) {
      if (entry.pass->name() == name) {
        entry.enabled = true;
        return;
      }
    }
  }

  void Optimizer::disable_pass(std::string const& name) {
    for (auto& entry : passes) {
      if (entry.pass->name() == name) {
        entry.enabled = false;
        return;
      }
    }
  }

  void Optimizer::optimize(ByteCode& byte_code) {
    for (auto& entry : passes) {
      if (entry.enabled) {
        std::print(std::cout, "Running optimization pass: {}\n", entry.pass->name());
        entry.pass->apply(byte_code);
      }
    }
  }

}
