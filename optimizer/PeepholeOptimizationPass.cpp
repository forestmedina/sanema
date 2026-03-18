//
// Peephole optimization pass for Sanema bytecode
//

#include "PeepholeOptimizationPass.h"
#include <optional>
#include <print>
#include <iostream>

namespace sanema {

  namespace {

    struct TypeInfo {
      OPCODE push_opcode;
      OPCODE add_opcode;
      OPCODE subtract_opcode;
      OPCODE multiply_opcode;
      bool is_integer;
    };

    constexpr TypeInfo sint8_info{
      OPCODE::OP_PUSH_SINT8_CONST,
      OPCODE::OP_ADD_SINT8,
      OPCODE::OP_SUBTRACT_SINT8,
      OPCODE::OP_MULTIPLY_SINT8,
      true,
    };

    constexpr TypeInfo sint16_info{
      OPCODE::OP_PUSH_SINT16_CONST,
      OPCODE::OP_ADD_SINT16,
      OPCODE::OP_SUBTRACT_SINT16,
      OPCODE::OP_MULTIPLY_SINT16,
      true,
    };

    constexpr TypeInfo sint32_info{
      OPCODE::OP_PUSH_SINT32_CONST,
      OPCODE::OP_ADD_SINT32,
      OPCODE::OP_SUBTRACT_SINT32,
      OPCODE::OP_MULTIPLY_SINT32,
      true,
    };

    constexpr TypeInfo sint64_info{
      OPCODE::OP_PUSH_SINT64_CONST,
      OPCODE::OP_ADD_SINT64,
      OPCODE::OP_SUBTRACT_SINT64,
      OPCODE::OP_MULTIPLY_SINT64,
      true,
    };

    constexpr TypeInfo float_info{
      OPCODE::OP_PUSH_FLOAT_CONST,
      OPCODE::OP_ADD_FLOAT,
      OPCODE::OP_SUBTRACT_FLOAT,
      OPCODE::OP_MULTIPLY_FLOAT,
      false,
    };

    constexpr TypeInfo double_info{
      OPCODE::OP_PUSH_DOUBLE_CONST,
      OPCODE::OP_ADD_DOUBLE,
      OPCODE::OP_SUBTRACT_DOUBLE,
      OPCODE::OP_MULTIPLY_DOUBLE,
      false,
    };

    constexpr TypeInfo type_infos[] = {
      sint8_info, sint16_info, sint32_info, sint64_info,
      float_info, double_info,
    };

    std::optional<TypeInfo const*> get_type_info_for_push(OPCODE opcode) {
      for (auto const& info : type_infos) {
        if (info.push_opcode == opcode) {
          return &info;
        }
      }
      return std::nullopt;
    }

    enum class ArithOp {
      ADD,
      SUBTRACT,
      MULTIPLY,
    };

    struct MatchedArithOp {
      TypeInfo const* type_info;
      ArithOp op;
    };

    std::optional<MatchedArithOp> get_arith_op(OPCODE opcode) {
      for (auto const& info : type_infos) {
        if (opcode == info.add_opcode) return MatchedArithOp{&info, ArithOp::ADD};
        if (opcode == info.subtract_opcode) return MatchedArithOp{&info, ArithOp::SUBTRACT};
        if (opcode == info.multiply_opcode) return MatchedArithOp{&info, ArithOp::MULTIPLY};
      }
      return std::nullopt;
    }

    // Check if a const_pool value is zero for the given type
    bool is_zero(OperandType const& value, TypeInfo const* info) {
      if (info == &sint8_info) return value.sint8_v == 0;
      if (info == &sint16_info) return value.sint16_v == 0;
      if (info == &sint32_info) return value.sint32_v == 0;
      if (info == &sint64_info) return value.sint64_v == 0;
      if (info == &float_info) return value.float_v == 0.0f;
      if (info == &double_info) return value.double_v == 0.0;
      return false;
    }

    // Check if a const_pool value is one for the given type
    bool is_one(OperandType const& value, TypeInfo const* info) {
      if (info == &sint8_info) return value.sint8_v == 1;
      if (info == &sint16_info) return value.sint16_v == 1;
      if (info == &sint32_info) return value.sint32_v == 1;
      if (info == &sint64_info) return value.sint64_v == 1;
      if (info == &float_info) return value.float_v == 1.0f;
      if (info == &double_info) return value.double_v == 1.0;
      return false;
    }

    // Check if a const_pool value is two for the given type (integer types only)
    bool is_two(OperandType const& value, TypeInfo const* info) {
      if (!info->is_integer) return false;
      if (info == &sint8_info) return value.sint8_v == 2;
      if (info == &sint16_info) return value.sint16_v == 2;
      if (info == &sint32_info) return value.sint32_v == 2;
      if (info == &sint64_info) return value.sint64_v == 2;
      return false;
    }

    std::optional<size_t> next_non_nop(std::vector<VMInstruction> const& code, size_t start) {
      for (size_t i = start; i < code.size(); ++i) {
        if (code[i].opcode != OPCODE::OP_NOP) {
          return i;
        }
      }
      return std::nullopt;
    }

  } // anonymous namespace

  bool PeepholeOptimizationPass::optimize_once(ByteCode& byte_code) {
    auto& code = byte_code.code_data;
    bool did_optimize = false;

    if (code.size() < 2) {
      return false;
    }

    size_t pos = 0;
    while (pos < code.size()) {
      // We look for a PUSH_*_CONST instruction followed (skipping NOPs) by an arithmetic op.
      // The const may be either operand (r1 or r2) of the arithmetic instruction.
      auto idx_push = next_non_nop(code, pos);
      if (!idx_push.has_value()) break;

      auto& inst_push = code[*idx_push];

      // Check if this is a PUSH_*_CONST
      auto push_type = get_type_info_for_push(inst_push.opcode);
      if (!push_type.has_value()) {
        pos = *idx_push + 1;
        continue;
      }

      // Find the next non-NOP instruction after the push
      auto idx_next = next_non_nop(code, *idx_push + 1);
      if (!idx_next.has_value()) break;

      auto& inst_next = code[*idx_next];

      // Check if the next instruction is an arithmetic op of the same type
      auto arith = get_arith_op(inst_next.opcode);
      if (!arith.has_value() || arith->type_info != *push_type) {
        pos = *idx_push + 1;
        continue;
      }

      // The push writes to r_result. Check if that register is used as r1 or r2 of the arith op.
      auto push_dest = inst_push.r_result;
      auto const_idx = inst_push.registers16.r1;
      auto const& const_value = byte_code.const_pool[const_idx];

      bool const_is_r1 = (inst_next.registers16.r1 == push_dest);
      bool const_is_r2 = (inst_next.registers16.r2 == push_dest);

      if (!const_is_r1 && !const_is_r2) {
        pos = *idx_push + 1;
        continue;
      }

      // Determine the "other" operand register (the non-constant one)
      std::uint16_t other_reg = const_is_r1 ? inst_next.registers16.r2 : inst_next.registers16.r1;
      bool other_is_ref = const_is_r1 ? inst_next.is_r2_reference : inst_next.is_r1_reference;

      // --- Identity: ADD x, 0 -> x  or  ADD 0, x -> x ---
      if (arith->op == ArithOp::ADD && is_zero(const_value, arith->type_info)) {
        // Replace the arith instruction with a NOP; the push_const becomes a NOP too.
        // The result should just be the other operand. We need to ensure the arith's
        // r_result gets the value from other_reg. We can't just NOP both if they are
        // different registers, so we re-use the push instruction to copy:
        // Turn PUSH_CONST into a PUSH_LOCAL of the other_reg's value to r_result of the arith.
        // Actually, the simplest approach: NOP the push_const, and replace the arith op
        // with a NOP only if other_reg == inst_next.r_result. Otherwise we can't easily
        // eliminate it without a MOV instruction. For safety, we only optimize when the
        // destination matches the other operand (common case), or we keep the arith and
        // just NOP the push.
        //
        // Simplest correct approach: NOP the push_const and NOP the arith, but only if
        // other_reg == arith r_result. Otherwise just NOP the approach won't work.
        // A more general approach: replace arith with a copy. But there's no generic
        // copy-register instruction per type. Let's just handle both cases:
        // Case 1: other_reg == r_result -> NOP both
        // Case 2: other_reg != r_result -> replace push_const with push of 0 (identity)
        //         ... actually we can just leave it. Let's only optimize Case 1 for now
        //         OR we can replace the arith with the same-typed ADD with both operands
        //         being other_reg (ADD x, x would double it - wrong).
        //         We just NOP the push and keep the arith as ADD x, 0 = x... wait that
        //         doesn't help since the 0 is gone.
        //
        // Better approach: NOP the PUSH_CONST. Replace the arith instruction with the
        // appropriate PUSH_LOCAL_* that reads other_reg into r_result.
        // But PUSH_LOCAL reads from a stack offset, not a register in the same sense.
        //
        // Actually, looking at the VM more carefully: the arithmetic instructions read
        // from stack addresses stored in r1/r2 and write to r_result. The PUSH_CONST
        // writes the const value to the stack at r_result. So the "other_reg" is a stack
        // address that already has the value we want.
        //
        // If r_result of the arith == other_reg, then the value is already in place.
        // If r_result != other_reg, we can't easily copy without a typed instruction.
        //
        // For the general case, we can use OP_COPY_MEMORY to copy from other_reg to
        // r_result. Let's check: OP_COPY_MEMORY exists in the opcode set.
        // We just need to know its register encoding.
        //
        // Simplest safe approach for now: only optimize when r_result == other_reg,
        // which is a very common case (the compiler often reuses the slot).
        // For the general case, we convert the arith into OP_COPY_MEMORY.

        if (inst_next.r_result == other_reg) {
          // Value already in the right place, NOP both
          inst_push.opcode = OPCODE::OP_NOP;
          inst_next.opcode = OPCODE::OP_NOP;
          did_optimize = true;
        } else {
          // Use OP_COPY_MEMORY to move other_reg -> r_result
          inst_push.opcode = OPCODE::OP_NOP;
          inst_next.opcode = OPCODE::OP_COPY_MEMORY;
          inst_next.registers16.r1 = other_reg;
          inst_next.is_r1_reference = other_is_ref;
          // r_result stays the same (destination)
          did_optimize = true;
        }
        pos = *idx_push + 1;
        continue;
      }

      // --- Identity: SUBTRACT x, 0 -> x (only when 0 is r2, i.e. the subtrahend) ---
      if (arith->op == ArithOp::SUBTRACT && const_is_r2 && is_zero(const_value, arith->type_info)) {
        if (inst_next.r_result == other_reg) {
          inst_push.opcode = OPCODE::OP_NOP;
          inst_next.opcode = OPCODE::OP_NOP;
          did_optimize = true;
        } else {
          inst_push.opcode = OPCODE::OP_NOP;
          inst_next.opcode = OPCODE::OP_COPY_MEMORY;
          inst_next.registers16.r1 = other_reg;
          inst_next.is_r1_reference = other_is_ref;
          did_optimize = true;
        }
        pos = *idx_push + 1;
        continue;
      }

      // --- Identity: MULTIPLY x, 1 -> x  or  MULTIPLY 1, x -> x ---
      if (arith->op == ArithOp::MULTIPLY && is_one(const_value, arith->type_info)) {
        if (inst_next.r_result == other_reg) {
          inst_push.opcode = OPCODE::OP_NOP;
          inst_next.opcode = OPCODE::OP_NOP;
          did_optimize = true;
        } else {
          inst_push.opcode = OPCODE::OP_NOP;
          inst_next.opcode = OPCODE::OP_COPY_MEMORY;
          inst_next.registers16.r1 = other_reg;
          inst_next.is_r1_reference = other_is_ref;
          did_optimize = true;
        }
        pos = *idx_push + 1;
        continue;
      }

      // --- Strength reduction: MULTIPLY x, 2 -> ADD x, x (integer types only) ---
      if (arith->op == ArithOp::MULTIPLY && is_two(const_value, arith->type_info)) {
        // NOP the push_const, replace MULTIPLY with ADD x, x
        inst_push.opcode = OPCODE::OP_NOP;
        inst_next.opcode = arith->type_info->add_opcode;
        inst_next.registers16.r1 = other_reg;
        inst_next.registers16.r2 = other_reg;
        inst_next.is_r1_reference = other_is_ref;
        inst_next.is_r2_reference = other_is_ref;
        // r_result stays the same
        did_optimize = true;
        pos = *idx_push + 1;
        continue;
      }

      pos = *idx_push + 1;
    }

    return did_optimize;
  }

  void PeepholeOptimizationPass::apply(ByteCode& byte_code) {
    while (optimize_once(byte_code)) {
      // Continue until no more peephole optimizations apply
    }
    std::print(std::cout, "Peephole optimization pass complete\n");
  }

  std::string PeepholeOptimizationPass::name() const {
    return "peephole_optimization";
  }

}
