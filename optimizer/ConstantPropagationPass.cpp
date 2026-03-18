//
// Constant propagation optimization pass for Sanema bytecode
//

#include "ConstantPropagationPass.h"
#include <iostream>
#include <print>
#include <unordered_map>
#include <unordered_set>

namespace sanema {

  namespace {

    struct ConstantInfo {
      OPCODE push_const_opcode;
      uint16_t const_pool_index;
    };

    struct TypeMapping {
      OPCODE push_const_opcode;
      OPCODE set_local_opcode;
      OPCODE push_local_opcode;
    };

    constexpr TypeMapping type_mappings[] = {
      {OPCODE::OP_PUSH_SINT8_CONST, OPCODE::OP_SET_LOCAL_SINT8, OPCODE::OP_PUSH_LOCAL_SINT8},
      {OPCODE::OP_PUSH_SINT16_CONST, OPCODE::OP_SET_LOCAL_SINT16, OPCODE::OP_PUSH_LOCAL_SINT16},
      {OPCODE::OP_PUSH_SINT32_CONST, OPCODE::OP_SET_LOCAL_SINT32, OPCODE::OP_PUSH_LOCAL_SINT32},
      {OPCODE::OP_PUSH_SINT64_CONST, OPCODE::OP_SET_LOCAL_SINT64, OPCODE::OP_PUSH_LOCAL_SINT64},
      {OPCODE::OP_PUSH_FLOAT_CONST, OPCODE::OP_SET_LOCAL_FLOAT, OPCODE::OP_PUSH_LOCAL_FLOAT},
      {OPCODE::OP_PUSH_DOUBLE_CONST, OPCODE::OP_SET_LOCAL_DOUBLE, OPCODE::OP_PUSH_LOCAL_DOUBLE},
      {OPCODE::OP_PUSH_STRING_CONST, OPCODE::OP_SET_LOCAL_STRING, OPCODE::OP_PUSH_LOCAL_STRING},
    };

    bool is_push_const(OPCODE op) {
      for (auto const& m : type_mappings) {
        if (m.push_const_opcode == op) return true;
      }
      return false;
    }

    bool is_set_local(OPCODE op) {
      for (auto const& m : type_mappings) {
        if (m.set_local_opcode == op) return true;
      }
      // Also check bool
      return op == OPCODE::OP_SET_LOCAL_BOOL;
    }

    bool is_push_local(OPCODE op) {
      for (auto const& m : type_mappings) {
        if (m.push_local_opcode == op) return true;
      }
      return op == OPCODE::OP_PUSH_LOCAL_BOOL;
    }

    // Given a SET_LOCAL opcode, find which PUSH_CONST opcode corresponds to it
    OPCODE push_const_for_set_local(OPCODE set_op) {
      for (auto const& m : type_mappings) {
        if (m.set_local_opcode == set_op) return m.push_const_opcode;
      }
      return OPCODE::OP_NOP;
    }

    // Given a PUSH_LOCAL opcode, find which PUSH_CONST opcode corresponds to it
    OPCODE push_const_for_push_local(OPCODE push_op) {
      for (auto const& m : type_mappings) {
        if (m.push_local_opcode == push_op) return m.push_const_opcode;
      }
      return OPCODE::OP_NOP;
    }

    // Given a SET_LOCAL opcode, find the matching PUSH_LOCAL opcode
    OPCODE push_local_for_set_local(OPCODE set_op) {
      for (auto const& m : type_mappings) {
        if (m.set_local_opcode == set_op) return m.push_local_opcode;
      }
      if (set_op == OPCODE::OP_SET_LOCAL_BOOL) return OPCODE::OP_PUSH_LOCAL_BOOL;
      return OPCODE::OP_NOP;
    }

    bool is_branch_or_label_target(OPCODE op) {
      return op == OPCODE::OP_JUMP ||
             op == OPCODE::OP_JUMP_IF_FALSE ||
             op == OPCODE::OP_JUMP_BACK;
    }

    bool is_call(OPCODE op) {
      return op == OPCODE::OP_CALL ||
             op == OPCODE::OP_CALL_EXTERNAL_FUNCTION ||
             op == OPCODE::OP_CALL_YIELDABLE_FUNCTION;
    }

    // Collect all jump target addresses so we know which instructions are branch targets
    std::unordered_set<uint32_t> collect_jump_targets(std::vector<VMInstruction> const& code) {
      std::unordered_set<uint32_t> targets;
      for (size_t i = 0; i < code.size(); ++i) {
        auto const& inst = code[i];
        if (inst.opcode == OPCODE::OP_JUMP || inst.opcode == OPCODE::OP_JUMP_IF_FALSE) {
          // Jump forward: target is register32.r1 (absolute address)
          targets.insert(inst.register32.r1);
        } else if (inst.opcode == OPCODE::OP_JUMP_BACK) {
          targets.insert(inst.register32.r1);
        }
      }
      return targets;
    }

  } // anonymous namespace

  bool ConstantPropagationPass::propagate_once(ByteCode& byte_code) {
    auto& code = byte_code.code_data;
    bool changed = false;

    if (code.size() < 2) {
      return false;
    }

    auto jump_targets = collect_jump_targets(code);

    // Map from local register -> constant info
    // Tracks which locals are known to hold a constant value
    std::unordered_map<uint16_t, ConstantInfo> constant_locals;

    // Track the last PUSH_*_CONST destination register and its info,
    // so when we see a SET_LOCAL we can associate the local with the constant
    struct PendingConst {
      uint16_t dest_register;
      OPCODE push_const_opcode;
      uint16_t const_pool_index;
    };
    std::optional<PendingConst> pending_const;

    for (size_t i = 0; i < code.size(); ++i) {
      auto& inst = code[i];

      if (inst.opcode == OPCODE::OP_NOP) {
        continue;
      }

      // If this instruction is a branch target, invalidate all mappings
      // (value may differ depending on which path was taken)
      if (jump_targets.contains(static_cast<uint32_t>(i))) {
        constant_locals.clear();
        pending_const.reset();
      }

      // If we hit a branch instruction, invalidate all mappings
      // (we're leaving this basic block)
      if (is_branch_or_label_target(inst.opcode)) {
        constant_locals.clear();
        pending_const.reset();
        continue;
      }

      // If we hit a call, invalidate all mappings (side effects)
      if (is_call(inst.opcode)) {
        constant_locals.clear();
        pending_const.reset();
        continue;
      }

      // Track PUSH_*_CONST instructions
      if (is_push_const(inst.opcode)) {
        pending_const = PendingConst{
          inst.r_result,
          inst.opcode,
          inst.registers16.r1
        };
        continue;
      }

      // When we see SET_LOCAL_*(dest=X, src=A), check if A came from a PUSH_*_CONST
      if (is_set_local(inst.opcode)) {
        OPCODE expected_push_const = push_const_for_set_local(inst.opcode);

        if (pending_const.has_value() &&
            inst.registers16.r1 == pending_const->dest_register &&
            expected_push_const == pending_const->push_const_opcode) {
          // Local X now holds a known constant
          constant_locals[inst.r_result] = ConstantInfo{
            pending_const->push_const_opcode,
            pending_const->const_pool_index
          };
        } else {
          // Local is being assigned a non-constant value; invalidate
          constant_locals.erase(inst.r_result);
        }
        pending_const.reset();
        continue;
      }

      // When we see PUSH_LOCAL_*(dest=D, src=X) and X is known constant,
      // replace with PUSH_*_CONST(dest=D, const_pool_index)
      if (is_push_local(inst.opcode)) {
        uint16_t local_reg = inst.registers16.r1;
        auto it = constant_locals.find(local_reg);
        if (it != constant_locals.end()) {
          OPCODE expected_push_const = push_const_for_push_local(inst.opcode);
          if (expected_push_const == it->second.push_const_opcode) {
            // Replace PUSH_LOCAL with PUSH_*_CONST
            inst.opcode = it->second.push_const_opcode;
            inst.registers16.r1 = it->second.const_pool_index;
            // r_result stays the same (destination register)
            changed = true;
          }
        }
        pending_const.reset();
        continue;
      }

      // Any other instruction clears the pending const
      pending_const.reset();
    }

    return changed;
  }

  void ConstantPropagationPass::apply(ByteCode& byte_code) {
    while (propagate_once(byte_code)) {
      // Continue propagating until no more changes
    }
    std::print(std::cout, "Constant propagation pass complete\n");
  }

  std::string ConstantPropagationPass::name() const {
    return "constant_propagation";
  }

}
