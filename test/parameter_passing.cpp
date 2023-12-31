//
// Created by fores on 11/20/2023.
//
#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

TEST_CASE("passing literal and  return values  as const reference",
          "[parser]") {

std::string code(R"--(
var result string;
var string_a string;
set result concat ( concat("hola" ", ") "mundo" );
)--");
 auto result=run_and_get_stack_value<std::string>(code);
//
  REQUIRE(result.has_value());

  REQUIRE(result.value()=="hola, mundo");

}