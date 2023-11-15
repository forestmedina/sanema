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
namespace sanema{
  using BYTECodeIPType = std::uint8_t const *;

  template<typename T>
  T read_from_bytecode(BYTECodeIPType &ip) {
    constexpr std::uint64_t size = sizeof(T);
    T value = *((T *) ip);
    ip += size;
    return value;
  };

  template<>
  std::uint64_t read_from_bytecode(BYTECodeIPType &ip);

  template<class T>
    void write_to_byte_code(std::vector<std::uint8_t>& code_data,T const &t) {
      auto length = sizeof(t) / sizeof(uint8_t);
      for (int i = 0; i < length; i++) {
        code_data.emplace_back();
      }
      std::uint8_t *pointer_uint8 = &code_data[code_data.size() - length];
      T *pointer_t = (T *) pointer_uint8;
      (*pointer_t) = t;
    };
   template<>
    void write_to_byte_code(std::vector<std::uint8_t>& code_data, OPCODE const &opcode);

  struct ByteCode {
    std::vector<std::uint8_t> code_data;
    std::vector<std::string> string_literals;
    ByteCode()=default;
    ByteCode (ByteCode const& other)=delete;
    ByteCode& operator=(ByteCode const& other)=delete;
    ByteCode (ByteCode && other)=default;
    ByteCode & operator=(ByteCode && other)=default;
    template<class T>
    void write(T const &t) {
      write_to_byte_code(code_data,t);
    };



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
        auto opcode = read_from_bytecode<OPCODE>(ip);
        std::cout << opcode_to_string(opcode);
        switch (opcode) {
          case OPCODE::OP_PUSH_SINT64_CONST: {
            auto value = read_from_bytecode<std::int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_PUSH_SINT32_CONST: {
            auto value = read_from_bytecode<std::int64_t>(ip);
            std::cout << " " << value;
          }
            break;
          case OPCODE::OP_ADD_SINT64: {
          }
            break;
          case OPCODE::OP_MULTIPLY_SINT64: {
          }
            break;
          case OPCODE::OP_SUBTRACT_SINT64: {
          }
          case OPCODE::OP_DIVIDE_SINT64: {
          }
            break;
          case OPCODE::OP_POP: {
          }
            break;
          case OPCODE::OP_NEGATE_SINT64: {
          }
            break;
          case OPCODE::OP_TRUE: {
          }
            break;
          case OPCODE::OP_FALSE: {
          }
            break;
          case OPCODE::OP_RESERVE_STACK_SPACE: {
            auto address = read_from_bytecode<std::uint64_t>(ip);
            std::cout << " " << address;
          }
            break;
          case OPCODE::OP_PUSH_LOCAL_SINT64: {
            auto size = read_from_bytecode<std::uint64_t>(ip);
            std::cout << " " << size;
          }
            break;
          case OPCODE::OP_PUSH_LOCAL_SINT32: {
            auto size = read_from_bytecode<std::uint64_t>(ip);
            std::cout << " " << size;
          }
            break;
          case OPCODE::OP_POP_TO_LOCAL_SINT64: {
            auto address = read_from_bytecode<std::uint64_t>(ip);
            std::cout << " " << address;
          }
            break;
          case OPCODE::OP_SET_LOCAL_SINT64: {
            auto address = read_from_bytecode<std::uint64_t>(ip);
            std::cout << " " << address;
          }
            break;
          case OPCODE::OP_JUMP: {
            auto offset = read_from_bytecode<std::uint16_t>(ip);
            ip += offset;
          }
            break;
          case OPCODE::OP_JUMP_IF_FALSE: {
            auto offset = read_from_bytecode<std::uint16_t>(ip);
            std::cout << " " << offset;
          }
            break;
          case OPCODE::OP_GREATER_SINT64: {
          }
            break;
          case OPCODE::OP_LESS_SINT64: {
          }
            break;
          case OPCODE::OP_EQUAL_SINT64: {
          }
            break;
          case OPCODE::OP_GREATER_EQUAL_SINT64: {
          }
            break;
          case OPCODE::OP_LESS_EQUAL_SINT64: {
          }
            break;

        }
        std::cout << "    |\n";
      }
    }


  };

}
#endif //NATURE_BYTECODE_H
