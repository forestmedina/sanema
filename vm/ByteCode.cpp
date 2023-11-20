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
std::uint64_t sanema::write_to_byte_code(std::vector<std::uint8_t> &code_data, OPCODE const &opcode) {
  auto address=code_data.size();
  code_data.emplace_back(static_cast<typename std::underlying_type<OPCODE>::type>(opcode));
  return address;
}


std::uint64_t sanema::ByteCode::get_current_address() {
  return code_data.size();
}

