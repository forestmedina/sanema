//
// Created by fores on 11/17/2023.
//
#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

TEST_CASE("divide two integers same type",
          "[parser]") {

  std::string code(R"--(
var number1 int64;
var number2 int64;
set number1 10;
set number2 15;
set number1 divide (number1 number2);
)--");
  auto result=run_and_get_stack_value<std::uint64_t >(code);
  auto success=result.has_value() && result.value()==(10/15);
  REQUIRE(success);

}

TEST_CASE("divide two integers with different types",
          "[math]") {

  std::string code(R"--(
var number1 int64;
var number2 int16;
set number1 10;
set number2 15;
set number1  divide (number1 number2);
)--");
  auto result=run_and_get_stack_value<std::int64_t >(code);
  //
  REQUIRE(result.has_value());

  REQUIRE(result.value()==(10/15));

}
TEST_CASE("divide two floats ",
          "[math]")  {

  std::string code(R"--(
var number1 float;
var number2 float;
set number1 10.33;
set number2 15.65;
set number1 divide (number1 number2);
)--");
  auto result=run_and_get_stack_value<float >(code);

  REQUIRE(result.has_value());
  REQUIRE_THAT(result.value(),Catch::Matchers::WithinAbs((10.33/15.65),0.001));

}


TEST_CASE("divide two doubles ",
          "[math]")  {

  std::string code(R"--(
var number1 float;
var number2 float;
set number1 10.33;
set number2 15.65;
set number1 divide (number1 number2);
)--");
  auto result=run_and_get_stack_value<float >(code);

  REQUIRE(result.has_value());
  REQUIRE_THAT(result.value(),Catch::Matchers::WithinAbs((10.33/15.65),0.001));

}

TEST_CASE("divide double var and constant ",
          "[math]")  {

  std::string code(R"--(
var number1 double;
var number2 double;
set number1 10.33d;
set number1  divide (number1 10d);
)--");
  auto result=run_and_get_stack_value<double >(code);

  REQUIRE(result.has_value());
  REQUIRE_THAT(result.value(),Catch::Matchers::WithinAbs((10.33/10.0),0.001));

}