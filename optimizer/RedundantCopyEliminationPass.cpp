//
// Redundant copy elimination optimization pass for Sanema bytecode
//

#include "RedundantCopyEliminationPass.h"
#include <iostream>
#include <print>
#include <unordered_set>

namespace sanema {

  namespace {

    struct SetPushPair {
      OPCODE set_opcode;
      OPCODE push_opcode;
    };

    constexpr SetPushPair type_pairs[] = {
      {OPCODE::OP_SET_LOCAL_SINT8, OPCODE::OP_PUSH_LOCAL_SINT8},
      {OPCODE::OP_SET_LOCAL_SINT16, OPCODE::OP_PUSH_LOCAL_SINT16},
      {OPCODE::OP_SET_LOCAL_SINT32, OPCODE::OP_PUSH_LOCAL_SINT32},
      {OPCODE::OP_SET_LOCAL_SINT64, OPCODE::OP_PUSH_LOCAL_SINT64},
      {OPCODE::OP_SET_LOCAL_FLOAT, OPCODE::OP_PUSH_LOCAL_FLOAT},
      {OPCODE::OP_SET_LOCAL_DOUBLE, OPCODE::OP_PUSH_LOCAL_DOUBLE},
      {OPCODE::OP_SET_LOCAL_BOOL, OPCODE::OP_PUSH_LOCAL_BOOL},
      {OPCODE::OP_SET_LOCAL_STRING, OPCODE::OP_PUSH_LOCAL_STRING},
    };

    bool is_set_local(OPCODE op) {
      for (auto const& pair : type_pairs) {
        if (pair.set_opcode == op) return true;
      }
      return false;
    }

    bool is_push_local(OPCODE op) {
      for (auto const& pair : type_pairs) {
        if (pair.push_opcode == op) return true;
      }
      return false;
    }

    OPCODE matching_push_for_set(OPCODE set_op) {
      for (auto const& pair : type_pairs) {
        if (pair.set_opcode == set_op) return pair.push_opcode;
      }
      return OPCODE::OP_NOP;
    }

    OPCODE matching_set_for_push(OPCODE push_op) {
      for (auto const& pair : type_pairs) {
        if (pair.push_opcode == push_op) return pair.set_opcode;
      }
      return OPCODE::OP_NOP;
    }

    // Find the next non-NOP instruction index
    std::optional<size_t> next_non_nop(std::vector<VMInstruction> const& code, size_t start) {
      for (size_t i = start; i < code.size(); ++i) {
        if (code[i].opcode != OPCODE::OP_NOP) {
          return i;
        }
      }
      return std::nullopt;
    }

    // Check if a register is read between two positions (exclusive)
    bool is_register_read_between(std::vector<VMInstruction> const& code, uint16_t reg, size_t from, size_t to) {
      for (size_t i = from; i < to && i < code.size(); ++i) {
        if (code[i].opcode == OPCODE::OP_NOP) continue;
        // Check if any instruction reads from this register via r1 or r2
        if (is_push_local(code[i].opcode) && code[i].registers16.r1 == reg) {
          return true;
        }
      }
      return false;
    }

    // Check if a register is read after a given position
    bool is_register_read_after(std::vector<VMInstruction> const& code, uint16_t reg, size_t after) {
      for (size_t i = after + 1; i < code.size(); ++i) {
        if (code[i].opcode == OPCODE::OP_NOP) continue;
        if (is_push_local(code[i].opcode) && code[i].registers16.r1 == reg) {
          return true;
        }
        // If the register is overwritten before being read, it's not read
        if (is_set_local(code[i].opcode) && code[i].r_result == reg) {
          return false;
        }
        // Conservative: if we hit a jump/call/return, assume it might be read
        if (code[i].opcode == OPCODE::OP_JUMP ||
            code[i].opcode == OPCODE::OP_JUMP_IF_FALSE ||
            code[i].opcode == OPCODE::OP_JUMP_BACK ||
            code[i].opcode == OPCODE::OP_RETURN ||
            code[i].opcode == OPCODE::OP_CALL ||
            code[i].opcode == OPCODE::OP_YIELD) {
          return true; // conservative
        }
      }
      return false;
    }

  } // anonymous namespace

  bool RedundantCopyEliminationPass::eliminate_set_push_pairs(ByteCode& byte_code) {
    // Pattern: SET_LOCAL_T(dest=A, src=B) followed by PUSH_LOCAL_T(dest=C, src=A)
    // where A is not read again after the pair.
    // Optimization: NOP the SET_LOCAL, change PUSH_LOCAL to read from B instead of A.
    auto& code = byte_code.code_data;
    bool changed = false;

    for (size_t i = 0; i < code.size(); ++i) {
      if (code[i].opcode == OPCODE::OP_NOP) continue;
      if (!is_set_local(code[i].opcode)) continue;

      auto expected_push = matching_push_for_set(code[i].opcode);
      auto next = next_non_nop(code, i + 1);
      if (!next.has_value()) continue;

      auto& set_inst = code[i];
      auto& push_inst = code[*next];

      // Check if next instruction is PUSH_LOCAL of same type reading from set's destination
      if (push_inst.opcode != expected_push) continue;
      if (push_inst.registers16.r1 != set_inst.r_result) continue;

      // Check that the intermediate register (set_inst.r_result) is not read after this pair
      if (is_register_read_after(code, set_inst.r_result, *next)) continue;

      // Redirect push to read from set's source
      push_inst.registers16.r1 = set_inst.registers16.r1;
      // NOP out the SET_LOCAL
      set_inst.opcode = OPCODE::OP_NOP;
      changed = true;
    }

    return changed;
  }

  bool RedundantCopyEliminationPass::eliminate_push_set_push_chains(ByteCode& byte_code) {
    // Pattern: PUSH_LOCAL_T(dest=A, src=X) → SET_LOCAL_T(dest=Y, src=A) → PUSH_LOCAL_T(dest=B, src=Y)
    // where Y is only used in this chain.
    // Optimization: NOP the SET_LOCAL and second PUSH_LOCAL, redirect to use X directly.
    auto& code = byte_code.code_data;
    bool changed = false;

    for (size_t i = 0; i < code.size(); ++i) {
      if (code[i].opcode == OPCODE::OP_NOP) continue;
      if (!is_push_local(code[i].opcode)) continue;

      auto corresponding_set = matching_set_for_push(code[i].opcode);

      auto idx_b = next_non_nop(code, i + 1);
      if (!idx_b.has_value()) continue;

      auto idx_c = next_non_nop(code, *idx_b + 1);
      if (!idx_c.has_value()) continue;

      auto& push_a = code[i];
      auto& set_b = code[*idx_b];
      auto& push_c = code[*idx_c];

      // Verify: SET_LOCAL reads from push_a's result
      if (set_b.opcode != corresponding_set) continue;
      if (set_b.registers16.r1 != push_a.r_result) continue;

      // Verify: second PUSH_LOCAL reads from SET_LOCAL's destination
      if (push_c.opcode != push_a.opcode) continue;
      if (push_c.registers16.r1 != set_b.r_result) continue;

      // Check Y (set_b.r_result) is not used elsewhere
      if (is_register_read_after(code, set_b.r_result, *idx_c)) continue;

      // Redirect: second push reads from original source, NOP the middle SET
      push_c.registers16.r1 = push_a.registers16.r1;
      set_b.opcode = OPCODE::OP_NOP;
      // Keep push_a in case its result register (A) is used elsewhere
      // But if A is also not read, dead code elimination will clean it up
      changed = true;
    }

    return changed;
  }

  bool RedundantCopyEliminationPass::eliminate_return_double_copy(ByteCode& byte_code) {
    // Pattern around return: SET_LOCAL to a temp, then PUSH_LOCAL from temp into return register, then RETURN
    // This is the pattern noted in ByteCodeCompiler.cpp:967
    auto& code = byte_code.code_data;
    bool changed = false;

    for (size_t i = 0; i < code.size(); ++i) {
      if (code[i].opcode == OPCODE::OP_NOP) continue;
      if (!is_set_local(code[i].opcode)) continue;

      auto expected_push = matching_push_for_set(code[i].opcode);
      auto idx_push = next_non_nop(code, i + 1);
      if (!idx_push.has_value()) continue;

      auto idx_ret = next_non_nop(code, *idx_push + 1);
      if (!idx_ret.has_value()) continue;

      auto& set_inst = code[i];
      auto& push_inst = code[*idx_push];
      auto& ret_inst = code[*idx_ret];

      if (push_inst.opcode != expected_push) continue;
      if (push_inst.registers16.r1 != set_inst.r_result) continue;
      if (ret_inst.opcode != OPCODE::OP_RETURN) continue;

      // This is a return double-copy: redirect push to read from set's source
      push_inst.registers16.r1 = set_inst.registers16.r1;
      set_inst.opcode = OPCODE::OP_NOP;
      changed = true;
    }

    return changed;
  }

  void RedundantCopyEliminationPass::apply(ByteCode& byte_code) {
    bool changed = true;
    while (changed) {
      changed = false;
      changed |= eliminate_set_push_pairs(byte_code);
      changed |= eliminate_push_set_push_chains(byte_code);
      changed |= eliminate_return_double_copy(byte_code);
    }
    std::print(std::cout, "Redundant copy elimination pass complete\n");
  }

  std::string RedundantCopyEliminationPass::name() const {
    return "redundant_copy_elimination";
  }

}
