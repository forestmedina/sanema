//
// Created by fores on 11/1/2023.
//

#include "ByteCode.h"
#include <cassert>
#include <format>

std::uint64_t sanema::ByteCode::get_current_address() {
  return code_data.size();
}

void sanema::ByteCode::print() {
  BYTECodeIPType ip = code_data.data();
  std::cout << "byte code size =" << code_data.size() << "\n";
  while (ip < code_data.data() + code_data.size()) {
    auto offset = std::uint64_t(ip - code_data.data());
    auto vmInstruction = read_from_bytecode(ip);
    std::cout << std::format(
      " {}:{} r_result:{}{}  R1:{}{} R2:{}{}   ", offset, opcode_to_string(vmInstruction.opcode),
      vmInstruction.r_result, vmInstruction.is_rresult_reference ? "G" : "L", vmInstruction.is_r1_reference ? "G" : "L",
      vmInstruction.registers16.r1, vmInstruction.is_r2_reference ? "G" : "L", vmInstruction.registers16.r2
                            );
    std::cout << "    |\n";
  }
}

