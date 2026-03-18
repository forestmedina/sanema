//
// Tail call optimization pass for Sanema bytecode
//

#include "TailCallOptimizationPass.h"
#include <print>
#include <iostream>
#include <algorithm>
#include <vector>
#include <optional>

namespace sanema {

  namespace {

    // Find the next non-NOP instruction index starting from 'start'
    std::optional<size_t> next_non_nop(std::vector<VMInstruction> const& code, size_t start) {
      for (size_t i = start; i < code.size(); ++i) {
        if (code[i].opcode != OPCODE::OP_NOP) {
          return i;
        }
      }
      return std::nullopt;
    }

    struct FunctionRange {
      std::uint64_t entry_address;
      std::uint64_t parameters_size; // r_result of calls to this function
    };

  } // anonymous namespace

  void TailCallOptimizationPass::apply(ByteCode& byte_code) {
    auto& code = byte_code.code_data;
    if (code.size() < 2) {
      return;
    }

    // Build a set of known function entry addresses so we can detect self-calls.
    // We collect entry addresses from FunctionCollection.
    std::vector<FunctionRange> function_ranges;
    byte_code.function_collection.for_each_function(
        [&function_ranges](FunctionID /*id*/, FunctionDefinitionCompleted const& func) {
          // Only internal (non-external) functions have meaningful code addresses
          if (!func.external_id.has_value()) {
            function_ranges.push_back(FunctionRange{func.address, 0});
          }
        });

    // Sort by entry address so we can determine which function owns a given instruction
    std::sort(function_ranges.begin(), function_ranges.end(),
              [](FunctionRange const& a, FunctionRange const& b) {
                return a.entry_address < b.entry_address;
              });

    // For each function, find tail-call patterns: OP_CALL(self) followed by OP_RETURN
    // A self-call is an OP_CALL whose register32.r1 (target address) equals the entry
    // address of the function that contains the OP_CALL instruction.
    int optimized_count = 0;

    for (size_t i = 0; i < code.size(); ++i) {
      if (code[i].opcode != OPCODE::OP_CALL) {
        continue;
      }

      auto call_target = static_cast<std::uint64_t>(code[i].register32.r1);
      auto call_parameters_offset = code[i].r_result;

      // Determine which function contains this instruction by finding the
      // largest entry_address that is <= i
      auto owning_func_it = std::upper_bound(
          function_ranges.begin(), function_ranges.end(), i,
          [](size_t addr, FunctionRange const& fr) {
            return addr < fr.entry_address;
          });

      if (owning_func_it == function_ranges.begin()) {
        // Instruction is before the first function -- skip
        continue;
      }
      --owning_func_it;

      // Check if this is a self-recursive call
      if (call_target != owning_func_it->entry_address) {
        continue;
      }

      // Look for OP_RETURN immediately after (skipping NOPs)
      auto next_idx = next_non_nop(code, i + 1);
      if (!next_idx.has_value()) {
        continue;
      }

      if (code[*next_idx].opcode != OPCODE::OP_RETURN) {
        continue;
      }

      // We found a tail-call pattern: OP_CALL(self) + OP_RETURN
      // Replace OP_CALL with OP_JUMP_BACK to the function entry.
      // The jump offset is: current_position - target_address + 1
      // (OP_JUMP_BACK subtracts the offset from ip, and ip has already
      //  been incremented past this instruction when it executes, so
      //  we need offset = (i + 1) - entry_address to land on entry_address)
      auto jump_back_offset = static_cast<std::uint16_t>((i + 1) - owning_func_it->entry_address);

      // Replace the OP_CALL with OP_JUMP_BACK
      code[i].opcode = OPCODE::OP_JUMP_BACK;
      code[i].registers16.r1 = jump_back_offset;
      // Clear r_result and other fields that are no longer relevant
      code[i].r_result = 0;
      code[i].registers16.r2 = 0;

      // Replace the OP_RETURN with OP_NOP since we no longer return here
      code[*next_idx].opcode = OPCODE::OP_NOP;

      ++optimized_count;
    }

    if (optimized_count > 0) {
      std::print(std::cout, "Tail call optimization: optimized {} self-recursive tail call(s)\n",
                 optimized_count);
    }
  }

  std::string TailCallOptimizationPass::name() const {
    return "tail_call_optimization";
  }

}
