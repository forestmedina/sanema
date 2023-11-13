//
// Created by fores on 11/1/2023.
//
#pragma once
#ifndef NATURE_SANEMA_OPCODES_H
#define NATURE_SANEMA_OPCODES_H

#include <cstdint>
#include <unordered_map>
#include <string>
enum class OPCODE : std::uint8_t {
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_RESERVE_STACK_SPACE,
  OP_GET_GLOBAL,
  OP_DEFINE_GLOBAL,
  OP_SET_GLOBAL,
  OP_GET_UPVALUE,
  OP_SET_UPVALUE,
  OP_GET_PROPERTY,
  OP_SET_PROPERTY,
  OP_GET_SUPER,
  OP_EQUAL_SINT64,
  OP_GREATER_SINT64,
  OP_LESS_SINT64,
  OP_GREATER_EQUAL_SINT64,
  OP_LESS_EQUAL_SINT64,
  OP_PUSH_SINT64_CONST,
  OP_ADD_SINT64,

  OP_SUBTRACT_SINT64,
  OP_MULTIPLY_SINT64,
  OP_DIVIDE_SINT64,
  OP_NEGATE_SINT64,
  OP_SET_LOCAL_SINT64,
  OP_PUSH_LOCAL_SINT64,
  OP_POP_TO_LOCAL_SINT64,
  OP_EQUAL_SINT32,
  OP_GREATER_SINT32,

  OP_LESS_SINT32,
  OP_GREATER_EQUAL_SINT32,
  OP_LESS_EQUAL_SINT32,
  OP_PUSH_SINT32_CONST,
  OP_ADD_SINT32,
  OP_SUBTRACT_SINT32,
  OP_MULTIPLY_SINT32,
  OP_DIVIDE_SINT32,
  OP_NEGATE_SINT32,
  OP_SET_LOCAL_SINT32,
  OP_PUSH_LOCAL_SINT32,
  OP_POP_TO_LOCAL_SINT32,
  OP_EQUAL_SINT16,
  OP_GREATER_SINT16,
  OP_LESS_SINT16,
  OP_GREATER_EQUAL_SINT16,
  OP_LESS_EQUAL_SINT16,
  OP_PUSH_SINT16_CONST,
  OP_ADD_SINT16,
  OP_SUBTRACT_SINT16,
  OP_MULTIPLY_SINT16,
  OP_DIVIDE_SINT16,
  OP_NEGATE_SINT16,
  OP_SET_LOCAL_SINT16,
  OP_PUSH_LOCAL_SINT16,
  OP_POP_TO_LOCAL_SINT16,
  OP_EQUAL_SINT8,
  OP_GREATER_SINT8,
  OP_LESS_SINT8,
  OP_GREATER_EQUAL_SINT8,
  OP_LESS_EQUAL_SINT8,
  OP_PUSH_SINT8_CONST,
  OP_ADD_SINT8,
  OP_SUBTRACT_SINT8,
  OP_MULTIPLY_SINT8,
  OP_DIVIDE_SINT8,
  OP_NEGATE_SINT8,
  OP_SET_LOCAL_SINT8,
  OP_PUSH_LOCAL_SINT8,
  OP_POP_TO_LOCAL_SINT8,
  OP_EQUAL_FLOAT,
  OP_GREATER_FLOAT,
  OP_LESS_FLOAT,
  OP_GREATER_EQUAL_FLOAT,
  OP_LESS_EQUAL_FLOAT,
  OP_PUSH_FLOAT_CONST,
  OP_ADD_FLOAT,
  OP_SUBTRACT_FLOAT,
  OP_MULTIPLY_FLOAT,
  OP_DIVIDE_FLOAT,
  OP_NEGATE_FLOAT,
  OP_PUSH_LOCAL_FLOAT,
  OP_POP_TO_LOCAL_FLOAT,
  OP_SET_LOCAL_FLOAT,
  OP_EQUAL_DOUBLE,
  OP_GREATER_DOUBLE,
  OP_LESS_DOUBLE,
  OP_GREATER_EQUAL_DOUBLE,
  OP_LESS_EQUAL_DOUBLE,
  OP_PUSH_DOUBLE_CONST,
  OP_ADD_DOUBLE,
  OP_SUBTRACT_DOUBLE,
  OP_MULTIPLY_DOUBLE,
  OP_DIVIDE_DOUBLE,
  OP_NEGATE_DOUBLE,
  OP_PUSH_LOCAL_DOUBLE,
  OP_POP_TO_LOCAL_DOUBLE,
  OP_SET_LOCAL_DOUBLE,
  OP_PUSH_STRING_CONST,
  OP_SET_LOCAL_STRING,
  OP_PUSH_LOCAL_STRING,
  OP_POP_TO_LOCAL_STRING,
  OP_CALL_EXTERNAL_FUNCTION,
  OP_NOT,
  OP_PRINT,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_LOOP,
  OP_CALL,
  OP_INVOKE,
  OP_SUPER_INVOKE,
  OP_CLOSURE,
  OP_CLOSE_UPVALUE,
  OP_RETURN,
  OP_CLASS,
  OP_INHERIT,
  OP_METHOD
};

inline std::string opcode_to_string(OPCODE opcode) {
#define SANEMA_OPCODE_TO_STRING(x) {OPCODE::x,#x}
  std::unordered_map<OPCODE, std::string> mappings{
   SANEMA_OPCODE_TO_STRING(OP_NIL),
        SANEMA_OPCODE_TO_STRING(OP_TRUE),
        SANEMA_OPCODE_TO_STRING(OP_FALSE),
        SANEMA_OPCODE_TO_STRING(OP_POP),
        SANEMA_OPCODE_TO_STRING(OP_RESERVE_STACK_SPACE),
        SANEMA_OPCODE_TO_STRING(OP_GET_GLOBAL),
        SANEMA_OPCODE_TO_STRING(OP_DEFINE_GLOBAL),
        SANEMA_OPCODE_TO_STRING(OP_SET_GLOBAL),
        SANEMA_OPCODE_TO_STRING(OP_GET_UPVALUE),
        SANEMA_OPCODE_TO_STRING(OP_SET_UPVALUE),
        SANEMA_OPCODE_TO_STRING(OP_GET_PROPERTY),
        SANEMA_OPCODE_TO_STRING(OP_SET_PROPERTY),
        SANEMA_OPCODE_TO_STRING(OP_GET_SUPER),
        SANEMA_OPCODE_TO_STRING(OP_EQUAL_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_LESS_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_EQUAL_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_LESS_EQUAL_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_SINT64_CONST),
        SANEMA_OPCODE_TO_STRING(OP_ADD_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_SUBTRACT_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_MULTIPLY_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_DIVIDE_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_NEGATE_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_SET_LOCAL_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_LOCAL_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_POP_TO_LOCAL_SINT64),
        SANEMA_OPCODE_TO_STRING(OP_EQUAL_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_LESS_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_EQUAL_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_LESS_EQUAL_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_SINT32_CONST),
        SANEMA_OPCODE_TO_STRING(OP_ADD_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_SUBTRACT_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_MULTIPLY_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_DIVIDE_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_NEGATE_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_SET_LOCAL_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_LOCAL_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_POP_TO_LOCAL_SINT32),
        SANEMA_OPCODE_TO_STRING(OP_EQUAL_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_LESS_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_EQUAL_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_LESS_EQUAL_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_SINT16_CONST),
        SANEMA_OPCODE_TO_STRING(OP_ADD_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_SUBTRACT_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_MULTIPLY_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_DIVIDE_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_NEGATE_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_SET_LOCAL_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_LOCAL_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_POP_TO_LOCAL_SINT16),
        SANEMA_OPCODE_TO_STRING(OP_EQUAL_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_LESS_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_EQUAL_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_LESS_EQUAL_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_SINT8_CONST),
        SANEMA_OPCODE_TO_STRING(OP_ADD_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_SUBTRACT_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_MULTIPLY_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_DIVIDE_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_NEGATE_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_SET_LOCAL_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_LOCAL_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_POP_TO_LOCAL_SINT8),
        SANEMA_OPCODE_TO_STRING(OP_EQUAL_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_LESS_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_EQUAL_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_LESS_EQUAL_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_FLOAT_CONST),
        SANEMA_OPCODE_TO_STRING(OP_ADD_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_SUBTRACT_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_MULTIPLY_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_DIVIDE_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_NEGATE_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_LOCAL_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_POP_TO_LOCAL_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_SET_LOCAL_FLOAT),
        SANEMA_OPCODE_TO_STRING(OP_EQUAL_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_LESS_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_GREATER_EQUAL_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_LESS_EQUAL_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_DOUBLE_CONST),
        SANEMA_OPCODE_TO_STRING(OP_ADD_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_SUBTRACT_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_MULTIPLY_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_DIVIDE_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_NEGATE_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_LOCAL_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_POP_TO_LOCAL_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_SET_LOCAL_DOUBLE),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_STRING_CONST),
        SANEMA_OPCODE_TO_STRING(OP_SET_LOCAL_STRING),
        SANEMA_OPCODE_TO_STRING(OP_PUSH_LOCAL_STRING),
        SANEMA_OPCODE_TO_STRING(OP_POP_TO_LOCAL_STRING),
        SANEMA_OPCODE_TO_STRING(OP_CALL_EXTERNAL_FUNCTION),
        SANEMA_OPCODE_TO_STRING(OP_NOT),
        SANEMA_OPCODE_TO_STRING(OP_PRINT),
        SANEMA_OPCODE_TO_STRING(OP_JUMP),
        SANEMA_OPCODE_TO_STRING(OP_JUMP_IF_FALSE),
        SANEMA_OPCODE_TO_STRING(OP_LOOP),
        SANEMA_OPCODE_TO_STRING(OP_CALL),
        SANEMA_OPCODE_TO_STRING(OP_INVOKE),
        SANEMA_OPCODE_TO_STRING(OP_SUPER_INVOKE),
        SANEMA_OPCODE_TO_STRING(OP_CLOSURE),
        SANEMA_OPCODE_TO_STRING(OP_CLOSE_UPVALUE),
        SANEMA_OPCODE_TO_STRING(OP_RETURN),
        SANEMA_OPCODE_TO_STRING(OP_CLASS),
        SANEMA_OPCODE_TO_STRING(OP_INHERIT),
        SANEMA_OPCODE_TO_STRING(OP_METHOD)
  };
  return mappings[opcode];

}

#endif //NATURE_OPCODES_H
