//
// Created by fores on 11/17/2023.
//
#include "generators.h"
#include <util/lambda_visitor.hpp>

void
sanema::generate_equal(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                       std::vector<sanema::address_t> addresses, address_t return_address) {
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
                            default:
                              return OPCODE::OP_EQUAL_SINT64;
                          }
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
  byte_code.write(opcode);
  byte_code.write(return_address);
  byte_code.write(addresses[0]);
  byte_code.write(addresses[1]);
}

void
sanema::generate_less(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                      std::vector<sanema::address_t> addresses, address_t return_address) {
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
                            default:
                              return OPCODE::OP_LESS_SINT64;
                          }
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
  byte_code.write(opcode);
  byte_code.write(return_address);
  byte_code.write(addresses[0]);
  byte_code.write(addresses[1]);
}

void
sanema::generate_greater(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                         std::vector<sanema::address_t> addresses, address_t return_address) {
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
                            default:
                              return OPCODE::OP_GREATER_SINT64;
                          }
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
  byte_code.write(opcode);
  byte_code.write(return_address);
  byte_code.write(addresses[0]);
  byte_code.write(addresses[1]);
}

void sanema::generate_add(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                          std::vector<sanema::address_t> addresses, address_t return_address) {
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
                            default:
                              return OPCODE::OP_ADD_SINT64;
                          }
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
  byte_code.write(opcode);
  byte_code.write(return_address);
  byte_code.write(addresses[0]);
  byte_code.write(addresses[1]);
}

void
sanema::generate_subtract(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                          std::vector<sanema::address_t> addresses, address_t return_address) {
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
                            default:
                              return OPCODE::OP_SUBTRACT_SINT64;
                          }
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
  byte_code.write(opcode);
  byte_code.write(return_address);
  byte_code.write(addresses[0]);
  byte_code.write(addresses[1]);
}

void
sanema::generate_multiply(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                          std::vector<sanema::address_t> addresses, address_t return_address) {
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
                            default:
                              return OPCODE::OP_MULTIPLY_SINT64;
                          }
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
  byte_code.write(opcode);
  byte_code.write(return_address);
  byte_code.write(addresses[0]);
  byte_code.write(addresses[1]);
}

void
sanema::generate_divide(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,
                        std::vector<sanema::address_t> addresses, address_t return_address) {
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
                            default:
                              return OPCODE::OP_DIVIDE_SINT64;
                          }
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
  byte_code.write(opcode);
  byte_code.write(return_address);
  byte_code.write(addresses[0]);
  byte_code.write(addresses[1]);
}
