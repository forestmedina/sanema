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
#include <vm/VMInstruction.h>

#include <unordered_set>
#include <common/FunctionCollection.h>

namespace sanema {
  using BYTECodeIPType = VMInstruction const *;


  inline VMInstruction read_from_bytecode(BYTECodeIPType &ip) {
    VMInstruction value = *((VMInstruction *) ip);
    ++ip;
    return value;
  }



  inline std::uint64_t
  write_to_byte_code(std::vector<VMInstruction> &code_data, std::uint64_t address, VMInstruction instruction) {
    code_data[address] = instruction;
    return address;
  }


  inline std::uint64_t write_to_byte_code(std::vector<VMInstruction> &code_data, VMInstruction instruction) {

    auto address = code_data.size() - 1;
    code_data.emplace_back(instruction);
    return address;
  }


  struct ByteCode {
    std::vector<VMInstruction> code_data;
    std::vector<std::string> string_literals;
    std::vector<OperandType> const_pool;
    sanema::FunctionCollection function_collection;
    ByteCode() = default;

    ByteCode(ByteCode const &other) = delete;

    ByteCode &operator=(ByteCode const &other) = delete;

    ByteCode(ByteCode &&other) = default;

    ByteCode &operator=(ByteCode &&other) = default;


    inline std::uint64_t write(VMInstruction instruction) {
      auto address = get_current_address();
      write_to_byte_code(code_data,
                         instruction);
      return address;
    };
    [[nodiscard]] size_t add_const(OperandType value){
      const_pool.emplace_back(value);
      return const_pool.size()-1;
    }

    template<class T>
    void write_to_address(T const &t, std::uint64_t address) {
      write_to_byte_code(code_data,
                         address,
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

    [[maybe_unused]]  void print() ;


  };

}
#endif //NATURE_BYTECODE_H
