//
// Created by fores on 11/17/2023.
//

#ifndef SANEMA_GENERATORS_H
#define SANEMA_GENERATORS_H
#include <vm/ByteCode.h>
#include <optional>
#include <parsing/SanemaParser.hpp>
namespace sanema{
  void generate_add(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,std::vector<sanema::address_t> addresses,address_t return_address);
  void generate_equal(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,std::vector<sanema::address_t> addresses,address_t return_address);
  void generate_less(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,std::vector<sanema::address_t> addresses,address_t return_address);
  void generate_greater(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,std::vector<sanema::address_t> addresses,address_t return_address);
  void generate_multiply(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,std::vector<sanema::address_t> addresses,address_t return_address);



  void generate_divide(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,std::vector<sanema::address_t> addresses,address_t return_address);
  void generate_subtract(sanema::ByteCode &byte_code, std::optional<sanema::DefineFunction> const &function_definition,std::vector<sanema::address_t> addresses,address_t return_address);
}
#endif //SANEMA_GENERATORS_H
