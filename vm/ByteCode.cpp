//
// Created by fores on 11/1/2023.
//

#include "ByteCode.h"
#include <cassert>

template<>
std::uint64_t sanema::read_from_bytecode(sanema::BYTECodeIPType &ip) {
  constexpr std::uint64_t size = sizeof(std::uint64_t);
  std::uint64_t value = *((std::uint64_t *) ip);
  ip += size;
  return value;
}

template<>
void sanema::write_to_byte_code(std::vector<std::uint8_t> &code_data, OPCODE const &opcode) {
  code_data.emplace_back(static_cast<typename std::underlying_type<OPCODE>::type>(opcode));
}

