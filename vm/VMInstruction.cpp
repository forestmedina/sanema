//
// Created by fores on 12/4/2023.
//

#include "VMInstruction.h"

// Every field is initialized, not just the reference flags.
//
// opcode, r_result and the register union used to be left indeterminate, and
// several emission sites set only some of them - generate_return sets nothing
// but the opcode. The VM then reads r_result, r1 and r2 unconditionally for
// OP_RETURN and memcpys with them:
//
//     memcpy(operand_stack_pointer + to, operand_stack_pointer + from, size);
//
// On x86_64 the leftover stack bytes happened to be zeros, so the copy was a
// harmless no-op and the bug stayed invisible. On arm64 they are not, which
// showed up as 22 segfaults in sanema_tests with offsets like
// memcpy(sp+0, sp+41128, 27919). Reading an indeterminate value is undefined
// either way; the platform only decided whether it hurt.
sanema::VMInstruction::VMInstruction()
  : opcode{OPCODE::OP_NOP},
    is_rresult_reference{false},
    is_r1_reference{false},
    is_r2_reference{false},
    r_result{0},
    registers16{0, 0} {}
