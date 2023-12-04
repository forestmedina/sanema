//
// Created by fores on 11/17/2023.
//

#ifndef SANEMA_GENERATORS_H
#define SANEMA_GENERATORS_H
#include <vm/ByteCode.h>
#include <optional>
#include <parsing/SanemaParser.hpp>
namespace sanema{
  void generate_add(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition, std::vector<sanema::local_register_t> registers, local_register_t return_register);
  void generate_equal(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition, std::vector<sanema::local_register_t> registers, local_register_t return_register);
  void generate_less(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition, std::vector<sanema::local_register_t> registers, local_register_t return_register);
  void generate_greater(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition, std::vector<sanema::local_register_t> registers, local_register_t return_register);
  void generate_multiply(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition, std::vector<sanema::local_register_t> registers, local_register_t return_register);



  void generate_divide(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition, std::vector<sanema::local_register_t> registers, local_register_t return_register);
  void generate_subtract(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition, std::vector<sanema::local_register_t> registers, local_register_t return_register);
}
#endif //SANEMA_GENERATORS_H
