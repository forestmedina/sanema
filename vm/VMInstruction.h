//
// Created by fores on 12/4/2023.
//

#ifndef SANEMA_VMINSTRUCTION_H
#define SANEMA_VMINSTRUCTION_H
#include "opcodes.h"
namespace  sanema {
  struct VMInstruction {
      std::uint8_t opcode;
      bool is_global_address2:1;
      bool is_global_address3:1;
      std::uint16_t  address1;
      std::uint16_t  address2;
      std::uint16_t  address3;
  };
}


#endif //SANEMA_VMINSTRUCTION_H
