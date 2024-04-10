//
// Created by fores on 11/17/2023.
//
#include "generators.h"
#include <util/lambda_visitor.hpp>

void
sanema::generate_equal(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
                       std::vector<sanema::local_register_t> registers, local_register_t return_register) {
  OPCODE opcode = match(function_definition->parameters[0].type.value(),
                        [](sanema::Integer const &integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_EQUAL_SINT8;
                            case 16:
                              return OPCODE::OP_EQUAL_SINT16;
                            case 32:
                              return OPCODE::OP_EQUAL_SINT32;
                            case 64:
                              return OPCODE::OP_EQUAL_SINT64;
                          }
                          return OPCODE::OP_EQUAL_SINT64;
                        },
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_EQUAL_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_EQUAL_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_EQUAL_SINT64;
                        });
  VMInstruction instruction{};
  instruction.opcode = opcode;
  instruction.is_rresult_reference=return_register.is_reference;
  instruction.r_result = return_register.address;
  instruction.registers16.r1 = registers[0].address;
  instruction.is_r1_reference = registers[0].is_reference;
  instruction.registers16.r2 = registers[1].address;
  instruction.is_r2_reference = registers[1].is_reference;
  byte_code.write(instruction);

}

void
sanema::generate_less(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
                      std::vector<sanema::local_register_t> registers, local_register_t return_register) {
  OPCODE opcode = match(function_definition->parameters[0].type.value(),
                        [](sanema::Integer const &integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_LESS_SINT8;
                            case 16:
                              return OPCODE::OP_LESS_SINT16;
                            case 32:
                              return OPCODE::OP_LESS_SINT32;
                            case 64:
                              return OPCODE::OP_LESS_SINT64;
                          }
                          return OPCODE::OP_LESS_SINT64;
                        },
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_LESS_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_LESS_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_LESS_SINT64;
                        });
  VMInstruction instruction{};
  instruction.opcode = opcode;
  instruction.is_rresult_reference=return_register.is_reference;
  instruction.r_result = return_register.address;
  instruction.registers16.r1 = registers[0].address;
  instruction.is_r1_reference = registers[0].is_reference;
  instruction.registers16.r2 = registers[1].address;
  instruction.is_r2_reference = registers[1].is_reference;
    byte_code.write(instruction);
}

void
sanema::generate_greater(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
                         std::vector<sanema::local_register_t> registers, local_register_t return_register) {
  OPCODE opcode = match(function_definition->parameters[0].type.value(),
                        [](sanema::Integer const &integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_GREATER_SINT8;
                            case 16:
                              return OPCODE::OP_GREATER_SINT16;
                            case 32:
                              return OPCODE::OP_GREATER_SINT32;
                            case 64:
                              return OPCODE::OP_GREATER_SINT64;
                          }
                          return OPCODE::OP_GREATER_SINT64;
                        },
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_GREATER_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_GREATER_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_GREATER_SINT64;
                        });
 VMInstruction instruction{};
  instruction.opcode = opcode;
  instruction.is_rresult_reference=return_register.is_reference;
  instruction.r_result = return_register.address;
  instruction.registers16.r1 = registers[0].address;
  instruction.is_r1_reference = registers[0].is_reference;
  instruction.registers16.r2 = registers[1].address;
  instruction.is_r2_reference = registers[1].is_reference;
    byte_code.write(instruction);
}

void sanema::generate_add(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
                          std::vector<sanema::local_register_t> registers, local_register_t return_register) {
  OPCODE opcode = match(function_definition.value().type,
                        [](sanema::Integer const &integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_ADD_SINT8;
                            case 16:
                              return OPCODE::OP_ADD_SINT16;
                            case 32:
                              return OPCODE::OP_ADD_SINT32;
                            case 64:
                              return OPCODE::OP_ADD_SINT64;
                          }
                          return OPCODE::OP_ADD_SINT64;
                        },
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_ADD_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_ADD_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_ADD_SINT64;
                        });
   VMInstruction instruction{};
  instruction.opcode = opcode;
  instruction.is_rresult_reference=return_register.is_reference;
  instruction.r_result = return_register.address;
  instruction.registers16.r1 = registers[0].address;
  instruction.is_r1_reference = registers[0].is_reference;
  instruction.registers16.r2 = registers[1].address;
  instruction.is_r2_reference = registers[1].is_reference;
    byte_code.write(instruction);
}

void
sanema::generate_subtract(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
                          std::vector<sanema::local_register_t> registers, local_register_t return_register) {
  OPCODE opcode = match(function_definition.value().type,
                        [](sanema::Integer const &integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_SUBTRACT_SINT8;
                            case 16:
                              return OPCODE::OP_SUBTRACT_SINT16;
                            case 32:
                              return OPCODE::OP_SUBTRACT_SINT32;
                            case 64:
                              return OPCODE::OP_SUBTRACT_SINT64;
                          }
                          return OPCODE::OP_SUBTRACT_SINT64;
                        },
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_SUBTRACT_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_SUBTRACT_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_SUBTRACT_SINT64;
                        });
 VMInstruction instruction{};
  instruction.opcode = opcode;
  instruction.is_rresult_reference=return_register.is_reference;
  instruction.r_result = return_register.address;
  instruction.registers16.r1 = registers[0].address;
  instruction.is_r1_reference = registers[0].is_reference;
  instruction.registers16.r2 = registers[1].address;
  instruction.is_r2_reference = registers[1].is_reference;
    byte_code.write(instruction);
}

void
sanema::generate_multiply(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
                          std::vector<sanema::local_register_t> registers, local_register_t return_register) {
  OPCODE opcode = match(function_definition.value().type,
                        [](sanema::Integer const &integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_MULTIPLY_SINT8;
                            case 16:
                              return OPCODE::OP_MULTIPLY_SINT16;
                            case 32:
                              return OPCODE::OP_MULTIPLY_SINT32;
                            case 64:
                              return OPCODE::OP_MULTIPLY_SINT64;
                          }
                          return OPCODE::OP_MULTIPLY_SINT64;
                        },
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_MULTIPLY_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_MULTIPLY_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_MULTIPLY_SINT64;
                        });
   VMInstruction instruction{};
  instruction.opcode = opcode;
  instruction.is_rresult_reference=return_register.is_reference;
  instruction.r_result = return_register.address;
  instruction.registers16.r1 = registers[0].address;
  instruction.is_r1_reference = registers[0].is_reference;
  instruction.registers16.r2 = registers[1].address;
  instruction.is_r2_reference = registers[1].is_reference;
    byte_code.write(instruction);
}

void
sanema::generate_divide(sanema::ByteCode &byte_code, std::optional<sanema::FunctionDefinitionCompleted> const &function_definition,
                        std::vector<sanema::local_register_t> registers, local_register_t return_register) {
  OPCODE opcode = match(function_definition.value().type,
                        [](sanema::Integer const &integer) {
                          switch (integer.size) {
                            case 8:
                              return OPCODE::OP_DIVIDE_SINT8;
                            case 16:
                              return OPCODE::OP_DIVIDE_SINT16;
                            case 32:
                              return OPCODE::OP_DIVIDE_SINT32;
                            case 64:
                              return OPCODE::OP_DIVIDE_SINT64;
                          }
                          return OPCODE::OP_DIVIDE_SINT64;
                        },
                        [](sanema::Float const &integer) {
                          return OPCODE::OP_DIVIDE_FLOAT;
                        },
                        [](sanema::Double const &integer) {
                          return OPCODE::OP_DIVIDE_DOUBLE;
                        },
                        [](auto const &ignore) {
                          return OPCODE::OP_DIVIDE_SINT64;
                        });
  VMInstruction instruction{};
  instruction.opcode = opcode;
  instruction.is_rresult_reference=return_register.is_reference;
  instruction.r_result = return_register.address;
  instruction.registers16.r1 = registers[0].address;
  instruction.is_r1_reference = registers[0].is_reference;
  instruction.registers16.r2 = registers[1].address;
  instruction.is_r2_reference = registers[1].is_reference;
    byte_code.write(instruction);
}
