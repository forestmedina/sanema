#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <built-in/built_in_functions.h>
#include <binding/IYieldableFunction.h>
#include <sstream>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::pair<sanema::ByteCode, sanema::BindingCollection>
compile(std::string const &code,
        std::function<void(sanema::BindingCollection &)> setup = {}) {
  sanema::SanemaParser parser;
  std::stringstream ss(code);
  auto tokens = parser.tokenize(ss);
  auto block = parser.parse(tokens);

  sanema::ByteCodeCompiler compiler;
  sanema::FunctionCollection functions;
  sanema::TypeCollection types;
  sanema::BindingCollection bindings;
  sanema::add_built_in_functions(functions, bindings);

  if (setup) setup(bindings);

  compiler.process(block, functions, types);
  return {std::move(compiler.byte_code), std::move(bindings)};
}

// ---------------------------------------------------------------------------
// A simple IYieldableFunction that finishes on first tick
// ---------------------------------------------------------------------------

struct ImmediateYieldable : public sanema::IYieldableFunction {
  int return_value;
  explicit ImmediateYieldable(int v) : return_value(v) {}
  void tick() override {}
  bool finished() override { return true; }
  void write_return(sanema::VM &vm) override {
    vm.push_function_return(std::int64_t{return_value});
  }
};

// ---------------------------------------------------------------------------
// A countdown yieldable that completes after N ticks
// ---------------------------------------------------------------------------

struct CountdownYieldable : public sanema::IYieldableFunction {
  int ticks_remaining;
  int return_value;
  CountdownYieldable(int ticks, int ret) : ticks_remaining(ticks), return_value(ret) {}
  void tick() override { --ticks_remaining; }
  bool finished() override { return ticks_remaining <= 0; }
  void write_return(sanema::VM &vm) override {
    vm.push_function_return(std::int64_t{return_value});
  }
};

// ---------------------------------------------------------------------------
// Test 1 — single tick completion
// ---------------------------------------------------------------------------

TEST_CASE("Yieldable: single tick completes immediately", "[yieldable]") {
  std::string code = R"(
    var result int64;
    set result wait_one();
  )";

  auto [bc, bindings] = compile(code, [](sanema::BindingCollection &b) {
    b.add_yieldable_function<std::int64_t>(
      "wait_one",
      []() -> std::unique_ptr<sanema::IYieldableFunction> {
        return std::make_unique<ImmediateYieldable>(42);
      });
  });

  sanema::VM vm(1, 2048);
  auto result = vm.run(bc, bindings);

  // Should complete without yielding (finished on first tick)
  REQUIRE(!result.has_value());
}

// ---------------------------------------------------------------------------
// Test 2 — multi-tick suspension
// ---------------------------------------------------------------------------

TEST_CASE("Yieldable: multi-tick suspends for exactly N frames", "[yieldable]") {
  std::string code = R"(
    var result int64;
    set result wait_n();
  )";

  constexpr int N = 3;
  auto [bc, bindings] = compile(code, [](sanema::BindingCollection &b) {
    b.add_yieldable_function<std::int64_t>(
      "wait_n",
      []() -> std::unique_ptr<sanema::IYieldableFunction> {
        return std::make_unique<CountdownYieldable>(N, 7);
      });
  });

  sanema::VM vm(1, 2048);

  // First run — tick 1, not finished → yield
  auto state = vm.run(bc, bindings);
  REQUIRE(state.has_value());

  // Resume 2 more times (tick 2 and tick 3)
  for (int i = 1; i < N; ++i) {
    state = vm.resume(state.value(), bindings);
    if (i < N - 1) {
      REQUIRE(state.has_value()); // still suspended
    }
  }

  // Last resume — tick N, finished → continues to completion
  REQUIRE(!state.has_value());
}

// ---------------------------------------------------------------------------
// Test 3 — return value placed correctly after multi-tick
// ---------------------------------------------------------------------------

TEST_CASE("Yieldable: return value is correct after 3 ticks", "[yieldable]") {
  std::string code = R"(
    var result int64;
    set result wait_three();
  )";

  auto [bc, bindings] = compile(code, [](sanema::BindingCollection &b) {
    b.add_yieldable_function<std::int64_t>(
      "wait_three",
      []() -> std::unique_ptr<sanema::IYieldableFunction> {
        return std::make_unique<CountdownYieldable>(3, 99);
      });
  });

  sanema::VM vm(1, 2048);
  auto state = vm.run(bc, bindings);

  // Resume until done
  while (state.has_value()) {
    state = vm.resume(state.value(), bindings);
  }

  auto val = vm.get_value_stack<std::int64_t>();
  REQUIRE(val.has_value());
  REQUIRE(val.value() == 99);
}

// ---------------------------------------------------------------------------
// Test 4 — yieldable + script yield interleaved
// ---------------------------------------------------------------------------

TEST_CASE("Yieldable: interleaved with explicit yield", "[yieldable]") {
  std::string code = R"(
    var a int64;
    set a wait_two();
    yield;
    set a 100;
  )";

  auto [bc, bindings] = compile(code, [](sanema::BindingCollection &b) {
    b.add_yieldable_function<std::int64_t>(
      "wait_two",
      []() -> std::unique_ptr<sanema::IYieldableFunction> {
        return std::make_unique<CountdownYieldable>(2, 55);
      });
  });

  sanema::VM vm(1, 2048);

  // Frame 1: tick 1 of wait_two (not finished) → yield
  auto state = vm.run(bc, bindings);
  REQUIRE(state.has_value());

  // Frame 2: tick 2 of wait_two (finished), writes 55, then hits explicit yield
  state = vm.resume(state.value(), bindings);
  REQUIRE(state.has_value());

  // Frame 3: after explicit yield, set a 100, complete
  state = vm.resume(state.value(), bindings);
  REQUIRE(!state.has_value());
}

// ---------------------------------------------------------------------------
// Test 5 — multiple concurrent yieldables (two separate calls)
// ---------------------------------------------------------------------------

TEST_CASE("Yieldable: two concurrent yieldables with different tick counts", "[yieldable]") {
  std::string code = R"(
    var a int64;
    var b int64;
    set a short_wait();
    set b long_wait();
  )";

  auto [bc, bindings] = compile(code, [](sanema::BindingCollection &b) {
    b.add_yieldable_function<std::int64_t>(
      "short_wait",
      []() -> std::unique_ptr<sanema::IYieldableFunction> {
        return std::make_unique<CountdownYieldable>(1, 10);
      });
    b.add_yieldable_function<std::int64_t>(
      "long_wait",
      []() -> std::unique_ptr<sanema::IYieldableFunction> {
        return std::make_unique<CountdownYieldable>(2, 20);
      });
  });

  sanema::VM vm(1, 2048);

  // Frame 1: short_wait tick 1 (done → sets a=10), long_wait tick 1 (not done) → yield
  auto state = vm.run(bc, bindings);
  REQUIRE(state.has_value());

  // Frame 2: long_wait tick 2 (done → sets b=20) → completes
  state = vm.resume(state.value(), bindings);
  REQUIRE(!state.has_value());
}
