//
// Created by fores on 11/1/2023.
//

#include "ByteCode.h"
#include <cassert>



std::uint64_t sanema::ByteCode::get_current_address() {
  return code_data.size();
}

