//
// Created by fores on 12/4/2023.
//

#ifndef SANEMA_VMINSTRUCTION_H
#define SANEMA_VMINSTRUCTION_H

#include "opcodes.h"
#include <cstdint>

namespace sanema {

  struct Registers16 {
    std::uint16_t r1;
    std::uint16_t r2;
  };
  struct Register32 {
    std::uint32_t r1;
  };
  struct VMInstruction {
    VMInstruction();

    OPCODE opcode;
    bool is_r1_reference: 1 ;
    bool is_r2_reference: 1;
    std::uint16_t r_result;
    union {
      Registers16 registers16;
      Register32 register32;
    };
  };
}


#endif //SANEMA_VMINSTRUCTION_H
