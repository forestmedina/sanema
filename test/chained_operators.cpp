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

TEST_CASE("chained operator literal",
          "functions") {
  sanema::SanemaParser parser;
  sanema::ByteCodeCompiler compiler;
  std::string code(R"--(
    function chain_operator_literal float
      var input float
    begin
        var value float;
        set value multiply(add(input 1.0f) 0.5f);
        return value;
    end

)--");
  float argument= 0.3f;
  auto result = run_function_and_get_stack_value<float>(code,"chain_operator_literal",argument);

  REQUIRE(result.has_value());
  REQUIRE(result.value() == ((argument+1.0f)*0.5f));

}