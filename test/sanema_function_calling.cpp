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

TEST_CASE("complex function calculate distance function",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(
    var number int64;
    function distance float
       var x1 float
       var y1 float
       var x2 float
       var y2 float
    begin
      var  value float;
      return  sqrt(
                add(
                    multiply(subtract(x2 x1) subtract(x2 x1))
                    multiply(subtract(y2 y1) subtract(y2 y1))
                )
            );
    end;



)--");
  float x1=1.0f;
  float y1=1.0f;
  float x2=2.0f;
  float y2=2.0f;
  float distance= sqrt(
                (x2-x1)*(x2-x1)
                +
                (y2-y1)*(y2-y1)
            );
  auto result = run_function_and_get_stack_value<float>(code, "distance", x1,y1,x2,y2);
  REQUIRE(result.has_value());
  REQUIRE(result.value() == distance);
}

TEST_CASE("return literal",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(
    function return_literal int64
    begin
      return 10;
    end;



)--");

  auto result = run_function_and_get_stack_value<std::uint64_t>(code, "return_literal");
  REQUIRE(result.has_value());
  REQUIRE(result.value() == 10);
}

TEST_CASE("return operator result",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(
    function return_operator int64
    begin
      return add(5 5);
    end;



)--");

  auto result = run_function_and_get_stack_value<std::uint64_t>(code, "return_operator");
  REQUIRE(result.has_value());
  REQUIRE(result.value() == 10);
}
TEST_CASE("return function call result",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(

    function return_operator int64
    begin
      return add(5 5);
    end;
    function return_function int64
    begin
      return return_operator();
    end;


)--");

  auto result = run_function_and_get_stack_value<std::uint64_t>(code, "return_function");
  REQUIRE(result.has_value());
  REQUIRE(result.value() == 10);
}

TEST_CASE("native reference modification",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(


    function return_function int64
        mut value int64
    begin
       set value add(value 5);
      return 0;
    end;


)--");

  std::int64_t value = 5;
  //TODO: when the a mut parameter is passed we should pass the address but we are passing the value
  //this need to be fixed I am forcing a fail so it does not crash
  REQUIRE(value==10);
  auto result = run_function_and_get_stack_value<std::int64_t>(code, "return_function", value);
  REQUIRE(value == 10);
}