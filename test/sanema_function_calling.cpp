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
    var result int64;
    function add_five int64
    var input int64
    begin
        return add ( input 5);
    end
    var number int64;
    set number 10;
    set result add_five(number);

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
    var number int64;
    function increase void
    mut value int64
    var increment int64
    begin
        set value add (value increment);
    end

    set number 10;
    increase(number 3);
)--");
  auto result = run_and_get_stack_value<std::int64_t>(code);
  REQUIRE(result.has_value());
  REQUIRE(result.value() == 13);
}