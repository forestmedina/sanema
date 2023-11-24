//
// Created by fores on 11/23/2023.
//
#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

TEST_CASE("calling  function by value",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(
    function add_five int64
    var input int64
    {
        return add ( input 5);
    };
    var number int64;
    set number 10;
    add_five(number);

)--");
  auto result = run_and_get_stack_value<std::uint64_t>(code);
  REQUIRE(result.has_value());
  REQUIRE(result.value() == 15);
}

TEST_CASE("calling  function by mutable reference",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(
    function increase void
    mut value int64
    var increment int64
    {
        set value add (value increment);
    };
    function return_value int64
    const value int64{
        return value;
    }
    var number int64;
    set number 10;
    increase(number 3);
    return_value number;
)--");
  auto result = run_and_get_stack_value<std::int64_t>(code);
  REQUIRE(result.has_value());
  REQUIRE(result.value() == 13);
}