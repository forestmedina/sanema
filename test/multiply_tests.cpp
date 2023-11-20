//
// Created by fores on 11/17/2023.
//
#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

TEST_CASE("multiply two integers same type",
          "[parser]") {

  std::string code(R"--(
var number1 int64;
var number2 int64;
set number1 10;
set number2 15;
multiply number1 number2;
)--");
  auto result=run_and_get_stack_value<std::uint64_t >(code);
  auto success=result.has_value() && result.value()==25;
  REQUIRE(success);

}

TEST_CASE("multiply two integers with different types",
          "[math]") {

  std::string code(R"--(
var number1 int64;
var number2 int16;
set number1 10;
set number2 15;
multiply number1 number2;
)--");
  auto result=run_and_get_stack_value<std::int64_t >(code);
  //
  REQUIRE(result.has_value());

  REQUIRE(result.value()==25);

}
TEST_CASE("multiply two floats ",
          "[math]")  {

  std::string code(R"--(
var number1 float;
var number2 float;
set number1 10.33;
set number2 15.65;
multiply number1 number2;
)--");
  auto result=run_and_get_stack_value<float >(code);

  REQUIRE(result.has_value());
  REQUIRE_THAT(result.value(),Catch::Matchers::WithinAbs((10.33+15.65),0.001));

}


TEST_CASE("multiply two doubles ",
          "[math]")  {

  std::string code(R"--(
var number1 float;
var number2 float;
set number1 10.33;
set number2 15.65;
multiply number1 number2;
)--");
  auto result=run_and_get_stack_value<float >(code);

  REQUIRE(result.has_value());
  REQUIRE_THAT(result.value(),Catch::Matchers::WithinAbs((10.33+15.65),0.001));

}

TEST_CASE("multiply double var and constant ",
          "[math]")  {

  std::string code(R"--(
var number1 double;
var number2 double;
set number1 10.33d;
multiply number1 10d;
)--");
  auto result=run_and_get_stack_value<double >(code);

  REQUIRE(result.has_value());
  REQUIRE_THAT(result.value(),Catch::Matchers::WithinAbs((10.33+10.0),0.001));

}