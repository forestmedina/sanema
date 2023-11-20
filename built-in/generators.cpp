//
// Created by fores on 11/17/2023.
//
#include "generators.h"
#include <util/lambda_visitor.hpp>
void sanema::generate_add(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition) {
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
}
void sanema::generate_subtract(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition) {
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
}
void sanema::generate_multiply(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition) {
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
}
void sanema::generate_divide(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition) {
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
}
