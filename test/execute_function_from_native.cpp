//
// Created by fores on 3/25/2024.
//
//
// Created by fores on 11/23/2023.
//
#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

TEST_CASE("calling  function from native",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(
    function add_five int64
    var input int64
    begin
        print ("****** add_five ******");
        return add ( input 5);
    end
    function triple_add int64
      var a int64
      var b int64
      var c int64
    begin
    print ("---- add triple ******");
    print ("\n");
    print (a);
    print ("\n");
    print (b);
    print ("\n");
    print (c);
        return add (add ( a b) c);
    end
)--");
  std::int64_t argument= 10;
  auto result = run_function_and_get_stack_value<std::int64_t>(code,"add_five",argument);

  REQUIRE(result.has_value());
  REQUIRE(result.value() == 15);
  std::int64_t a= 1;
  std::int64_t b= 2;
  std::int64_t c= 3;
  auto result2 = run_function_and_get_stack_value<std::int64_t>(code,"triple_add",a,b,c);
  REQUIRE(result2.has_value());
  REQUIRE(result2.value() == 6);
}