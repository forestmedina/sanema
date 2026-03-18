//
// Dead code elimination optimization pass for Sanema bytecode
//

#include "DeadCodeEliminationPass.h"
#include <iostream>
#include <print>
#include <unordered_set>
#include <vector>

namespace sanema {

  namespace {

    // Check if an opcode is a SET_LOCAL variant (writes to a local register)
    bool is_set_local(OPCODE opcode) {
      switch (opcode) {
        case OPCODE::OP_SET_LOCAL_SINT8:
        case OPCODE::OP_SET_LOCAL_SINT16:
        case OPCODE::OP_SET_LOCAL_SINT32:
        case OPCODE::OP_SET_LOCAL_SINT64:
        case OPCODE::OP_SET_LOCAL_FLOAT:
        case OPCODE::OP_SET_LOCAL_DOUBLE:
        case OPCODE::OP_SET_LOCAL_BOOL:
        case OPCODE::OP_SET_LOCAL_STRING:
          return true;
        default:
          return false;
      }
    }

    // Check if an opcode is a PUSH_LOCAL variant (reads from a local register)
    bool is_push_local(OPCODE opcode) {
      switch (opcode) {
        case OPCODE::OP_PUSH_LOCAL_SINT8:
        case OPCODE::OP_PUSH_LOCAL_SINT16:
        case OPCODE::OP_PUSH_LOCAL_SINT32:
        case OPCODE::OP_PUSH_LOCAL_SINT64:
        case OPCODE::OP_PUSH_LOCAL_FLOAT:
        case OPCODE::OP_PUSH_LOCAL_DOUBLE:
        case OPCODE::OP_PUSH_LOCAL_BOOL:
        case OPCODE::OP_PUSH_LOCAL_STRING:
          return true;
        default:
          return false;
      }
    }

    // Check if an opcode is a control flow terminator (unconditional)
    bool is_unconditional_terminator(OPCODE opcode) {
      return opcode == OPCODE::OP_JUMP ||
             opcode == OPCODE::OP_RETURN;
    }

    // Check if an opcode is any kind of jump (forward or backward)
    bool is_jump(OPCODE opcode) {
      return opcode == OPCODE::OP_JUMP ||
             opcode == OPCODE::OP_JUMP_IF_FALSE ||
             opcode == OPCODE::OP_JUMP_BACK;
    }

    // Check if an instruction reads from a register via registers16.r1
    bool reads_r1_register(OPCODE opcode) {
      // Most instructions that use r1 as a source operand
      // Be conservative: if the opcode is not recognized, assume it reads r1
      switch (opcode) {
        case OPCODE::OP_NOP:
        case OPCODE::OP_TRUE:
        case OPCODE::OP_FALSE:
        case OPCODE::OP_RESERVE_STACK_SPACE:
        case OPCODE::OP_RETURN:
        case OPCODE::OP_YIELD:
          return false;
        default:
          return true;
      }
    }

    // Build the set of all instruction indices that are jump targets
    std::unordered_set<size_t> compute_jump_targets(std::vector<VMInstruction> const& code) {
      std::unordered_set<size_t> targets;
      for (size_t i = 0; i < code.size(); ++i) {
        auto const& inst = code[i];
        if (inst.opcode == OPCODE::OP_NOP) continue;

        if (inst.opcode == OPCODE::OP_JUMP || inst.opcode == OPCODE::OP_JUMP_IF_FALSE) {
          // Forward jump: ip is at instruction position, after reading the instruction
          // the VM does ip += offset, so target = i + 1 + offset
          auto offset = inst.registers16.r1;
          size_t target = i + 1 + offset;
          if (target < code.size()) {
            targets.insert(target);
          }
        } else if (inst.opcode == OPCODE::OP_JUMP_BACK) {
          // Backward jump: ip -= offset, so target = i + 1 - offset
          auto offset = inst.registers16.r1;
          if (i + 1 >= offset) {
            size_t target = i + 1 - offset;
            targets.insert(target);
          }
        } else if (inst.opcode == OPCODE::OP_CALL) {
          // Function calls jump to an address stored in the instruction;
          // be conservative and treat the call target as potentially anything.
          // We do NOT NOP out code that could be a call target.
          // The call target is stored in register32.r1 as an absolute address.
          auto target = inst.register32.r1;
          if (target < code.size()) {
            targets.insert(target);
          }
        }
      }
      return targets;
    }

  } // anonymous namespace

  void DeadCodeEliminationPass::eliminate_unreachable_code(ByteCode& byte_code) {
    auto& code = byte_code.code_data;
    if (code.empty()) return;

    auto jump_targets = compute_jump_targets(code);

    for (size_t i = 0; i < code.size(); ++i) {
      auto const& inst = code[i];
      if (inst.opcode == OPCODE::OP_NOP) continue;

      if (!is_unconditional_terminator(inst.opcode)) continue;

      // NOP out all following instructions until we hit a jump target or end
      for (size_t j = i + 1; j < code.size(); ++j) {
        if (code[j].opcode == OPCODE::OP_NOP) continue;

        // If this instruction is a jump target, stop -- it is reachable
        if (jump_targets.contains(j)) break;

        // This instruction is unreachable; NOP it out
        code[j].opcode = OPCODE::OP_NOP;
      }
    }
  }

  void DeadCodeEliminationPass::eliminate_dead_stores(ByteCode& byte_code) {
    auto& code = byte_code.code_data;
    if (code.empty()) return;

    auto jump_targets = compute_jump_targets(code);

    for (size_t i = 0; i < code.size(); ++i) {
      auto const& inst = code[i];
      if (inst.opcode == OPCODE::OP_NOP) continue;
      if (!is_set_local(inst.opcode)) continue;

      // The register being written to
      auto written_register = inst.r_result;

      // Scan forward to see if this register is read before being overwritten
      bool is_read = false;
      bool is_overwritten = false;

      for (size_t j = i + 1; j < code.size(); ++j) {
        auto const& next = code[j];
        if (next.opcode == OPCODE::OP_NOP) continue;

        // If we hit a jump target, be conservative -- the register might
        // be read from a path we cannot trace linearly
        if (jump_targets.contains(j)) {
          is_read = true; // conservative: assume it could be read
          break;
        }

        // If we hit any kind of jump or call, be conservative
        if (is_jump(next.opcode) ||
            next.opcode == OPCODE::OP_CALL ||
            next.opcode == OPCODE::OP_CALL_EXTERNAL_FUNCTION ||
            next.opcode == OPCODE::OP_CALL_YIELDABLE_FUNCTION ||
            next.opcode == OPCODE::OP_RETURN ||
            next.opcode == OPCODE::OP_YIELD) {
          is_read = true; // conservative
          break;
        }

        // Check if this instruction reads from the written register
        if (is_push_local(next.opcode) && next.registers16.r1 == written_register) {
          is_read = true;
          break;
        }

        // Check if any instruction uses the register as r1 or r2 source operand
        // (arithmetic ops, comparisons, etc. read r1 and r2)
        if (reads_r1_register(next.opcode)) {
          if (next.registers16.r1 == written_register || next.registers16.r2 == written_register) {
            is_read = true;
            break;
          }
        }

        // Check if the register is overwritten by another SET_LOCAL
        if (is_set_local(next.opcode) && next.r_result == written_register) {
          is_overwritten = true;
          break;
        }
      }

      if (is_overwritten && !is_read) {
        // The store is dead -- NOP it out
        code[i].opcode = OPCODE::OP_NOP;
      }
    }
  }

  void DeadCodeEliminationPass::apply(ByteCode& byte_code) {
    // First eliminate unreachable code (after unconditional jumps/returns)
    eliminate_unreachable_code(byte_code);

    // Then eliminate dead stores
    eliminate_dead_stores(byte_code);

    std::print(std::cout, "Dead code elimination pass complete\n");
  }

  std::string DeadCodeEliminationPass::name() const {
    return "dead_code_elimination";
  }

}
