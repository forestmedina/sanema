//
// Created by fores on 11/1/2023.
//

#ifndef NATURE_BYTECODE_H
#define NATURE_BYTECODE_H

#include <cstdint>
#include <vector>
#include "opcodes.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <types.h>
#include <vm/OperandType.h>

namespace sanema {
  using BYTECodeIPType = std::uint8_t const *;

  template<typename T>
  T read_from_bytecode(BYTECodeIPType &ip) {
    constexpr std::uint64_t size = sizeof(T);
    T value = *((T *) ip);
    ip += size;
    return value;
  }
  template<typename T,typename TDEST>
  void read_from_bytecode_into(BYTECodeIPType &ip,TDEST* dest) {
    constexpr std::uint64_t size = sizeof(T);
    (*dest) = *((T *) ip);
    ip += size;
  }

  template<>
  std::uint64_t read_from_bytecode(BYTECodeIPType &ip);

  template<class T>
  std::uint64_t write_to_byte_code(std::vector<std::uint8_t> &code_data, std::uint64_t address, T const &t) {
    std::uint8_t *pointer_uint8 = &code_data[address];
    T *pointer_t = (T *) pointer_uint8;
    (*pointer_t) = t;
    return address;
  }

  template<class T>
  std::uint64_t write_to_byte_code(std::vector<std::uint8_t> &code_data, T const &t) {
    auto length = sizeof(t) / sizeof(uint8_t);
    auto address = code_data.size() - 1;
    for (int i = 0; i < length; i++) {
      code_data.emplace_back();
    }
    std::uint8_t *pointer_uint8 = &code_data[code_data.size() - length];

    T *pointer_t = (T *) pointer_uint8;
    (*pointer_t) = t;
    return address;
  }

  template<>
  std::uint64_t write_to_byte_code(std::vector<std::uint8_t> &code_data, OPCODE const &t);

  struct ByteCode {
    std::vector<std::uint8_t> code_data;
    std::vector<std::string> string_literals;


    ByteCode() = default;

    ByteCode(ByteCode const &other) = delete;

    ByteCode &operator=(ByteCode const &other) = delete;

    ByteCode(ByteCode &&other) = default;

    ByteCode &operator=(ByteCode &&other) = default;

    template<class T>
    std::uint64_t write(T const &t) {
      return write_to_byte_code(code_data,
                                t);
    };

    std::uint64_t get_current_address();


    size_t add_string_literal(std::string &string_literal) {
      auto iter = std::find(string_literals.begin(),
                            string_literals.end(),
                            string_literal);
      if (iter == string_literals.end()) {
        string_literals.emplace_back(string_literal);
        return string_literals.size() - 1;
      } else {
        return std::distance(string_literals.begin(),
                             iter);
      }
    }

    inline void print() {
      BYTECodeIPType ip = code_data.data();
      std::cout << "byte code size =" << code_data.size() << "\n";
      while (ip < code_data.data() + code_data.size()) {
        auto offset = std::uint64_t(ip - code_data.data());
        auto opcode = read_from_bytecode<OPCODE>(ip);
        std::cout << offset << " : " << opcode_to_string(opcode);
        switch (opcode) {
          case OPCODE::OP_POP: {
          }
            break;
          case OPCODE::OP_RESERVE_STACK_SPACE: {
            auto size = read_from_bytecode<std::uint64_t>(ip);
            std::cout << "reserving space =" << size;
          }
            break;
          case OPCODE::OP_TRUE: {
          }
            break;
          case OPCODE::OP_FALSE: {
          }
            break;

          case OPCODE::OP_PUSH_STRING_CONST: {
            auto string_literal_index = read_from_bytecode<sanema::StringReference>(ip);
            std::cout << string_literal_index;
          }
            break;
          case OPCODE::OP_JUMP: {
            auto offset = read_from_bytecode<std::uint64_t>(ip);
            std::cout<<" Offset: "<<offset<<"\n";
          }
            break;
          case OPCODE::OP_CALL_EXTERNAL_FUNCTION: {
            auto function_id = read_from_bytecode<std::uint64_t>(ip);
            std::cout << "id:" << function_id;
          }
            break;
          case OPCODE::OP_CALL: {
            auto function_address = read_from_bytecode<std::uint64_t>(ip);
            std::cout << "function address: " << function_address;
          }
            break;
          case OPCODE::OP_PREPARE_PARAMETER: {
            auto address = read_from_bytecode<std::uint64_t>(ip);
            std::cout << " address: " << address;

          }
            break;
          case OPCODE::OP_JUMP_IF_FALSE: {
            auto offset = read_from_bytecode<std::uint64_t>(ip);
            std::cout << " offset: " << offset;
          }
          case OPCODE::OP_RETURN:
            break;
          case OPCODE::OP_NIL:
            break;

          case OPCODE::OP_EQUAL_SINT64:
            break;
          case OPCODE::OP_GREATER_SINT64:
            break;
          case OPCODE::OP_LESS_SINT64:
            break;
          case OPCODE::OP_GREATER_EQUAL_SINT64:
            break;
          case OPCODE::OP_LESS_EQUAL_SINT64:
            break;
          case OPCODE::OP_PUSH_SINT64_CONST: {
            auto value = read_from_bytecode<int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_ADD_SINT64:
            break;
          case OPCODE::OP_SUBTRACT_SINT64:
            break;
          case OPCODE::OP_MULTIPLY_SINT64:
            break;
          case OPCODE::OP_DIVIDE_SINT64:
            break;
          case OPCODE::OP_NEGATE_SINT64:
            break;
          case OPCODE::OP_SET_LOCAL_SINT64:
            break;
          case OPCODE::OP_PUSH_LOCAL_SINT64: {
          }
            break;
          case OPCODE::OP_SET_EXTERNAL_SINT64: {
            auto value = read_from_bytecode<int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_EQUAL_SINT32:
            break;
          case OPCODE::OP_GREATER_SINT32:
            break;
          case OPCODE::OP_LESS_SINT32:
            break;
          case OPCODE::OP_GREATER_EQUAL_SINT32:
            break;
          case OPCODE::OP_LESS_EQUAL_SINT32:
            break;
          case OPCODE::OP_PUSH_SINT32_CONST: {
            auto value = read_from_bytecode<int32_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_ADD_SINT32:
            break;
          case OPCODE::OP_SUBTRACT_SINT32:
            break;
          case OPCODE::OP_MULTIPLY_SINT32:
            break;
          case OPCODE::OP_DIVIDE_SINT32:
            break;
          case OPCODE::OP_NEGATE_SINT32:
            break;
          case OPCODE::OP_SET_LOCAL_SINT32:
            break;
          case OPCODE::OP_PUSH_LOCAL_SINT32: {
          }
            break;
          case OPCODE::OP_SET_EXTERNAL_SINT32: {
            auto value = read_from_bytecode<int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_EQUAL_SINT16:
            break;
          case OPCODE::OP_GREATER_SINT16:
            break;
          case OPCODE::OP_LESS_SINT16:
            break;
          case OPCODE::OP_GREATER_EQUAL_SINT16:
            break;
          case OPCODE::OP_LESS_EQUAL_SINT16:
            break;
          case OPCODE::OP_PUSH_SINT16_CONST: {
            auto value = read_from_bytecode<int16_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_ADD_SINT16:
            break;
          case OPCODE::OP_SUBTRACT_SINT16:
            break;
          case OPCODE::OP_MULTIPLY_SINT16:
            break;
          case OPCODE::OP_DIVIDE_SINT16:
            break;
          case OPCODE::OP_NEGATE_SINT16:
            break;
          case OPCODE::OP_SET_LOCAL_SINT16:
            break;
          case OPCODE::OP_PUSH_LOCAL_SINT16: {
          }
            break;
          case OPCODE::OP_SET_EXTERNAL_SINT16: {
            auto value = read_from_bytecode<int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_EQUAL_SINT8:
            break;
          case OPCODE::OP_GREATER_SINT8:
            break;
          case OPCODE::OP_LESS_SINT8:
            break;
          case OPCODE::OP_GREATER_EQUAL_SINT8:
            break;
          case OPCODE::OP_LESS_EQUAL_SINT8:
            break;
          case OPCODE::OP_PUSH_SINT8_CONST: {
            auto value = read_from_bytecode<int8_t>(ip);
            std::cout << " " << (int) value;
          }
            break;
          case OPCODE::OP_ADD_SINT8:
            break;
          case OPCODE::OP_SUBTRACT_SINT8:
            break;
          case OPCODE::OP_MULTIPLY_SINT8:
            break;
          case OPCODE::OP_DIVIDE_SINT8:
            break;
          case OPCODE::OP_NEGATE_SINT8:
            break;
          case OPCODE::OP_SET_LOCAL_SINT8:
            break;
          case OPCODE::OP_PUSH_LOCAL_SINT8: {

          }
            break;
          case OPCODE::OP_SET_EXTERNAL_SINT8: {
            auto value = read_from_bytecode<int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_EQUAL_FLOAT:
            break;
          case OPCODE::OP_GREATER_FLOAT:
            break;
          case OPCODE::OP_LESS_FLOAT:
            break;
          case OPCODE::OP_GREATER_EQUAL_FLOAT:
            break;
          case OPCODE::OP_LESS_EQUAL_FLOAT:
            break;
          case OPCODE::OP_PUSH_FLOAT_CONST: {
            auto value = read_from_bytecode<float>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_ADD_FLOAT:
            break;
          case OPCODE::OP_SUBTRACT_FLOAT:
            break;
          case OPCODE::OP_MULTIPLY_FLOAT:
            break;
          case OPCODE::OP_DIVIDE_FLOAT:
            break;
          case OPCODE::OP_NEGATE_FLOAT:
            break;
          case OPCODE::OP_PUSH_LOCAL_FLOAT: {

          }
            break;
          case OPCODE::OP_SET_EXTERNAL_FLOAT:
            break;
          case OPCODE::OP_SET_LOCAL_FLOAT:
            break;
          case OPCODE::OP_EQUAL_DOUBLE:
            break;
          case OPCODE::OP_GREATER_DOUBLE:
            break;
          case OPCODE::OP_LESS_DOUBLE:
            break;
          case OPCODE::OP_GREATER_EQUAL_DOUBLE:
            break;
          case OPCODE::OP_LESS_EQUAL_DOUBLE:
            break;
          case OPCODE::OP_PUSH_DOUBLE_CONST: {
            auto value = read_from_bytecode<double>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_ADD_DOUBLE:
            break;
          case OPCODE::OP_SUBTRACT_DOUBLE:
            break;
          case OPCODE::OP_MULTIPLY_DOUBLE:
            break;
          case OPCODE::OP_DIVIDE_DOUBLE:
            break;
          case OPCODE::OP_NEGATE_DOUBLE:
            break;
          case OPCODE::OP_PUSH_LOCAL_DOUBLE: {
          }
            break;
          case OPCODE::OP_SET_EXTERNAL_DOUBLE: {
            auto value = read_from_bytecode<int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_SET_LOCAL_DOUBLE:
            break;
          case OPCODE::OP_SET_LOCAL_STRING:
            break;
          case OPCODE::OP_PUSH_LOCAL_STRING: {
          }
            break;
          case OPCODE::OP_SET_EXTERNAL_STRING: {
            auto value = read_from_bytecode<int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_NOT:
            break;
          case OPCODE::OP_PRINT:
            break;
          case OPCODE::OP_INVOKE:
            break;
          case OPCODE::OP_SUPER_INVOKE:
            break;
          case OPCODE::OP_CLOSURE:
            break;
          case OPCODE::OP_CLOSE_UPVALUE:
            break;
          case OPCODE::OP_CLASS:
            break;
          case OPCODE::OP_INHERIT:
            break;
          case OPCODE::OP_METHOD:
            break;
          case OPCODE::OP_PUSH_LOCAL_ADDRESS_AS_GLOBAL:
            read_from_bytecode<sanema::address_t>(ip);
            break;
          case OPCODE::OP_PUSH_EXTERNAL_FIELD_ADDRESS:
            break;
          case OPCODE::OP_POP_GLOBAL_ADDRESS_AS_LOCAL:
            break;
        }
        std::cout << "    |\n";
      }
    }


  };

}
#endif //NATURE_BYTECODE_H
