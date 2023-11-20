//
// Created by forest on 16/11/23.
//
#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

TEST_CASE("concatenting two strings",
          "[parser]") {

std::string code(R"--(
var a string;
var b string;
set a "hola, ";
set b "mundo";
concat a b;
)--");
  auto result=run_and_get_stack_value<std::string>(code);
//
  REQUIRE(result.has_value());

  REQUIRE(result.value()=="hola, mundo");

}
TEST_CASE("ends_with",
          "[parser]") {

std::string code(R"--(
var a string;
var b string;
set a "hola, ";
set b "mundo";
set a concat (a b);
ends_with(a "mundo")
)--");
  auto result=run_and_get_stack_value<bool>(code);
//
  REQUIRE(result.has_value());

  REQUIRE(result.value());
  std::string code2(R"--(
var a string;
var b string;
set a "hola, ";
set b "mundo";
set a concat (a b);
ends_with(a "cundo");
)--");
 auto result2=run_and_get_stack_value<bool>(code2);
//
  REQUIRE(result2.has_value());

  REQUIRE(!result2.value());
}