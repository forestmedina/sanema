#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <built-in/built_in_functions.h>
#include <sstream>

TEST_CASE("Yield execution", "[yield]") {
    std::string code = R"(
        var a int64;
        set a 10;
        yield;
        set a 20;
    )";

    sanema::SanemaParser parser;
    std::stringstream ss(code);
    auto tokens = parser.tokenize(ss);
    auto block = parser.parse(tokens);

    sanema::ByteCodeCompiler compiler;
    sanema::FunctionCollection functions;
    sanema::TypeCollection types;
    sanema::BindingCollection bindings;
    sanema::add_built_in_functions(functions, bindings);

    compiler.process(block, functions, types);

    sanema::VM vm(1, 2048);
    auto result = vm.run(compiler.byte_code, bindings);

    REQUIRE(result.has_value()); // Should return ExecutionState
    REQUIRE(result->page_index >= 0);

    // Check value of 'a' (should be 10)
    auto val = vm.get_value_stack<std::int64_t>();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 10);

    // Resume execution
    auto result2 = vm.resume(result.value(), bindings);

    // Should finish (return nullopt)
    REQUIRE(!result2.has_value());

    // Check value of 'a' (should be 20)
    val = vm.get_value_stack<std::int64_t>();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 20);
}

TEST_CASE("Yield inside repeat loop", "[yield]") {
    std::string code = R"(
        var a int64;
        set a 10;
        repeat i 10
            yield;
        set a 20;
    )";

    sanema::SanemaParser parser;
    std::stringstream ss(code);
    auto tokens = parser.tokenize(ss);
    auto block = parser.parse(tokens);

    sanema::ByteCodeCompiler compiler;
    sanema::FunctionCollection functions;
    sanema::TypeCollection types;
    sanema::BindingCollection bindings;
    sanema::add_built_in_functions(functions, bindings);

    compiler.process(block, functions, types);

    sanema::VM vm(1, 2048);
    auto result = vm.run(compiler.byte_code, bindings);

    REQUIRE(result.has_value());
    REQUIRE(result->page_index >= 0);

    // Check value of 'a' before loop completes (should be 10)
    auto val = vm.get_value_stack<std::int64_t>();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 10);

    // Resume 9 more times (yield runs 10 times total, first run hit yield once)
    for (int i = 1; i < 10; ++i) {
        result = vm.resume(result.value(), bindings);
        REQUIRE(result.has_value());
    }

    // Final resume should complete execution
    auto final_result = vm.resume(result.value(), bindings);
    REQUIRE(!final_result.has_value());

    // Check value of 'a' after loop (should be 20)
    val = vm.get_value_stack<std::int64_t>();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 20);
}
