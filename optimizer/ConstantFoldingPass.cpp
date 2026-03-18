//
// Constant folding optimization pass for Sanema bytecode
//

#include "ConstantFoldingPass.h"
#include <optional>
#include <print>
#include <iostream>

namespace sanema {

  namespace {

    struct PushConstInfo {
      OPCODE push_opcode;
      OPCODE add_opcode;
      OPCODE subtract_opcode;
      OPCODE multiply_opcode;
      OPCODE divide_opcode;
      OPCODE equal_opcode;
      OPCODE greater_opcode;
      OPCODE less_opcode;
      OPCODE greater_equal_opcode;
      OPCODE less_equal_opcode;
    };

    constexpr PushConstInfo sint8_info{
      OPCODE::OP_PUSH_SINT8_CONST,
      OPCODE::OP_ADD_SINT8,
      OPCODE::OP_SUBTRACT_SINT8,
      OPCODE::OP_MULTIPLY_SINT8,
      OPCODE::OP_DIVIDE_SINT8,
      OPCODE::OP_EQUAL_SINT8,
      OPCODE::OP_GREATER_SINT8,
      OPCODE::OP_LESS_SINT8,
      OPCODE::OP_GREATER_EQUAL_SINT8,
      OPCODE::OP_LESS_EQUAL_SINT8,
    };

    constexpr PushConstInfo sint16_info{
      OPCODE::OP_PUSH_SINT16_CONST,
      OPCODE::OP_ADD_SINT16,
      OPCODE::OP_SUBTRACT_SINT16,
      OPCODE::OP_MULTIPLY_SINT16,
      OPCODE::OP_DIVIDE_SINT16,
      OPCODE::OP_EQUAL_SINT16,
      OPCODE::OP_GREATER_SINT16,
      OPCODE::OP_LESS_SINT16,
      OPCODE::OP_GREATER_EQUAL_SINT16,
      OPCODE::OP_LESS_EQUAL_SINT16,
    };

    constexpr PushConstInfo sint32_info{
      OPCODE::OP_PUSH_SINT32_CONST,
      OPCODE::OP_ADD_SINT32,
      OPCODE::OP_SUBTRACT_SINT32,
      OPCODE::OP_MULTIPLY_SINT32,
      OPCODE::OP_DIVIDE_SINT32,
      OPCODE::OP_EQUAL_SINT32,
      OPCODE::OP_GREATER_SINT32,
      OPCODE::OP_LESS_SINT32,
      OPCODE::OP_GREATER_EQUAL_SINT32,
      OPCODE::OP_LESS_EQUAL_SINT32,
    };

    constexpr PushConstInfo sint64_info{
      OPCODE::OP_PUSH_SINT64_CONST,
      OPCODE::OP_ADD_SINT64,
      OPCODE::OP_SUBTRACT_SINT64,
      OPCODE::OP_MULTIPLY_SINT64,
      OPCODE::OP_DIVIDE_SINT64,
      OPCODE::OP_EQUAL_SINT64,
      OPCODE::OP_GREATER_SINT64,
      OPCODE::OP_LESS_SINT64,
      OPCODE::OP_GREATER_EQUAL_SINT64,
      OPCODE::OP_LESS_EQUAL_SINT64,
    };

    constexpr PushConstInfo float_info{
      OPCODE::OP_PUSH_FLOAT_CONST,
      OPCODE::OP_ADD_FLOAT,
      OPCODE::OP_SUBTRACT_FLOAT,
      OPCODE::OP_MULTIPLY_FLOAT,
      OPCODE::OP_DIVIDE_FLOAT,
      OPCODE::OP_EQUAL_FLOAT,
      OPCODE::OP_GREATER_FLOAT,
      OPCODE::OP_LESS_FLOAT,
      OPCODE::OP_GREATER_EQUAL_FLOAT,
      OPCODE::OP_LESS_EQUAL_FLOAT,
    };

    constexpr PushConstInfo double_info{
      OPCODE::OP_PUSH_DOUBLE_CONST,
      OPCODE::OP_ADD_DOUBLE,
      OPCODE::OP_SUBTRACT_DOUBLE,
      OPCODE::OP_MULTIPLY_DOUBLE,
      OPCODE::OP_DIVIDE_DOUBLE,
      OPCODE::OP_EQUAL_DOUBLE,
      OPCODE::OP_GREATER_DOUBLE,
      OPCODE::OP_LESS_DOUBLE,
      OPCODE::OP_GREATER_EQUAL_DOUBLE,
      OPCODE::OP_LESS_EQUAL_DOUBLE,
    };

    constexpr PushConstInfo type_infos[] = {
      sint8_info, sint16_info, sint32_info, sint64_info,
      float_info, double_info,
    };

    std::optional<PushConstInfo const*> get_type_info_for_push(OPCODE opcode) {
      for (auto const& info : type_infos) {
        if (info.push_opcode == opcode) {
          return &info;
        }
      }
      return std::nullopt;
    }

    enum class ArithmeticOp {
      ADD,
      SUBTRACT,
      MULTIPLY,
      DIVIDE,
    };

    struct FoldableArithmeticOp {
      PushConstInfo const* type_info;
      ArithmeticOp op;
    };

    std::optional<FoldableArithmeticOp> get_arithmetic_op(OPCODE opcode) {
      for (auto const& info : type_infos) {
        if (opcode == info.add_opcode) return FoldableArithmeticOp{&info, ArithmeticOp::ADD};
        if (opcode == info.subtract_opcode) return FoldableArithmeticOp{&info, ArithmeticOp::SUBTRACT};
        if (opcode == info.multiply_opcode) return FoldableArithmeticOp{&info, ArithmeticOp::MULTIPLY};
        if (opcode == info.divide_opcode) return FoldableArithmeticOp{&info, ArithmeticOp::DIVIDE};
      }
      return std::nullopt;
    }

    template<typename T>
    T evaluate_arithmetic(T lhs, T rhs, ArithmeticOp op) {
      switch (op) {
        case ArithmeticOp::ADD: return lhs + rhs;
        case ArithmeticOp::SUBTRACT: return lhs - rhs;
        case ArithmeticOp::MULTIPLY: return lhs * rhs;
        case ArithmeticOp::DIVIDE: return rhs != T{0} ? lhs / rhs : T{0};
      }
      return T{0};
    }

    OperandType compute_folded_value(
      OperandType const& lhs,
      OperandType const& rhs,
      ArithmeticOp op,
      PushConstInfo const* type_info
    ) {
      if (type_info == &sint8_info) {
        return OperandType{evaluate_arithmetic(lhs.sint8_v, rhs.sint8_v, op)};
      } else if (type_info == &sint16_info) {
        return OperandType{evaluate_arithmetic(lhs.sint16_v, rhs.sint16_v, op)};
      } else if (type_info == &sint32_info) {
        return OperandType{evaluate_arithmetic(lhs.sint32_v, rhs.sint32_v, op)};
      } else if (type_info == &sint64_info) {
        return OperandType{evaluate_arithmetic(lhs.sint64_v, rhs.sint64_v, op)};
      } else if (type_info == &float_info) {
        return OperandType{evaluate_arithmetic(lhs.float_v, rhs.float_v, op)};
      } else if (type_info == &double_info) {
        return OperandType{evaluate_arithmetic(lhs.double_v, rhs.double_v, op)};
      }
      return OperandType{std::int64_t{0}};
    }

    // Find the next non-NOP instruction index starting from `start`.
    // Returns std::nullopt if none found.
    std::optional<size_t> next_non_nop(std::vector<VMInstruction> const& code, size_t start) {
      for (size_t i = start; i < code.size(); ++i) {
        if (code[i].opcode != OPCODE::OP_NOP) {
          return i;
        }
      }
      return std::nullopt;
    }

  } // anonymous namespace

  bool ConstantFoldingPass::fold_once(ByteCode& byte_code) {
    auto& code = byte_code.code_data;
    bool did_fold = false;

    if (code.size() < 3) {
      return false;
    }

    size_t pos = 0;
    while (pos < code.size()) {
      // Find three consecutive non-NOP instructions
      auto idx_a = next_non_nop(code, pos);
      if (!idx_a.has_value()) break;

      auto idx_b = next_non_nop(code, *idx_a + 1);
      if (!idx_b.has_value()) break;

      auto idx_op = next_non_nop(code, *idx_b + 1);
      if (!idx_op.has_value()) break;

      auto& inst_a = code[*idx_a];
      auto& inst_b = code[*idx_b];
      auto& inst_op = code[*idx_op];

      // Both must be PUSH_*_CONST of the same type
      auto type_a = get_type_info_for_push(inst_a.opcode);
      if (!type_a.has_value()) {
        pos = *idx_a + 1;
        continue;
      }

      auto type_b = get_type_info_for_push(inst_b.opcode);
      if (!type_b.has_value()) {
        pos = *idx_a + 1;
        continue;
      }

      if (*type_a != *type_b) {
        pos = *idx_a + 1;
        continue;
      }

      // The third instruction must be an arithmetic op for the same type
      auto arith = get_arithmetic_op(inst_op.opcode);
      if (!arith.has_value() || arith->type_info != *type_a) {
        pos = *idx_a + 1;
        continue;
      }

      // Verify that the arithmetic op reads from the addresses written by the two pushes
      if (inst_op.registers16.r1 != inst_a.r_result ||
          inst_op.registers16.r2 != inst_b.r_result) {
        pos = *idx_a + 1;
        continue;
      }

      // Read operand values from const_pool
      auto const& lhs = byte_code.const_pool[inst_a.registers16.r1];
      auto const& rhs = byte_code.const_pool[inst_b.registers16.r1];

      // Compute folded value
      OperandType result = compute_folded_value(lhs, rhs, arith->op, arith->type_info);

      // Add new constant to pool
      auto new_const_idx = byte_code.add_const(result);

      // Replace the arithmetic op instruction with a push_const
      inst_op.opcode = arith->type_info->push_opcode;
      inst_op.registers16.r1 = static_cast<std::uint16_t>(new_const_idx);
      // r_result already holds the destination of the arithmetic op, which is correct

      // NOP out the two push instructions
      inst_a.opcode = OPCODE::OP_NOP;
      inst_b.opcode = OPCODE::OP_NOP;

      did_fold = true;
      pos = *idx_a + 1;
    }

    return did_fold;
  }

  void ConstantFoldingPass::apply(ByteCode& byte_code) {
    // Iterate until no more folds are possible (handles chained constant expressions)
    while (fold_once(byte_code)) {
      // Continue folding
    }

    // Remove NOP instructions to compact the bytecode
    // But we must be careful: jump targets are stored as instruction indices
    // in the bytecode. Removing instructions would invalidate those offsets.
    // So we leave the NOPs in place for safety.
    std::print(std::cout, "Constant folding pass complete\n");
  }

  std::string ConstantFoldingPass::name() const {
    return "constant_folding";
  }

}
