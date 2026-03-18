#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>
#include <SanemaScriptSystem.h>
#include <optimizer/Optimizer.h>

// ============================================================================
// Native C++ equivalents for VM overhead comparison
// ============================================================================

std::int64_t fib_native(std::int64_t n) {
  if (n == 0) {
    return n;
  } else {
    if (n == 1) {
      return n;
    } else {
      return fib_native(n - 1) +
             fib_native(n - 2);
    }
  }
}

std::int64_t cpp_arithmetic_heavy() {
  std::int64_t result = 0;
  result = ((((((((((std::int64_t{1} + 2) * 3) + 4) * 5) + 6) * 7) + 8) * 9) + 10) * 11);
  result = result + ((((((((((std::int64_t{11} + 12) * 13) + 14) * 15) + 16) * 17) + 18) * 19) + 20) * 21);
  result = result + ((((((((((std::int64_t{21} + 22) * 23) + 24) * 25) + 26) * 27) + 28) * 29) + 30) * 31);
  result = result + ((((((((((std::int64_t{31} + 32) * 33) + 34) * 35) + 36) * 37) + 38) * 39) + 40) * 41);
  return result;
}

std::int64_t cpp_loop_heavy() {
  std::int64_t accumulator = 0;
  for (std::int64_t i = 0; i < 1000; i++) {
    accumulator = accumulator + i;
  }
  return accumulator;
}

std::int64_t cpp_branch_heavy(std::int64_t value) {
  std::int64_t result = 0;
  if (value > 50) {
    if (value > 75) {
      if (value > 90) {
        result = 4;
      } else {
        result = 3;
      }
    } else {
      if (value > 60) {
        result = 2;
      } else {
        result = 1;
      }
    }
  } else {
    if (value > 25) {
      if (value > 40) {
        result = -1;
      } else {
        result = -2;
      }
    } else {
      if (value > 10) {
        result = -3;
      } else {
        result = -4;
      }
    }
  }
  return result;
}

std::int64_t cpp_mixed_realistic(std::int64_t health, std::int64_t armor, std::int64_t damage) {
  std::int64_t effective_damage = damage - armor / 2;
  if (effective_damage < 1) {
    effective_damage = 1;
  }
  std::int64_t new_health = health - effective_damage;
  if (new_health < 0) {
    new_health = 0;
  }
  std::int64_t score = 0;
  for (std::int64_t i = 0; i < 100; i++) {
    score = score + new_health + i;
  }
  return score;
}

// ============================================================================
// Fibonacci Benchmark (updated to use begin/end syntax)
// ============================================================================

// Iterative fibonacci - avoids known deep-recursion VM issue
static std::string const code_fibonacci = R"--(
function fib int64
    var n int64
begin
    if less(n 2)
        return n;
    end
    var prev int64;
    var curr int64;
    set prev 0;
    set curr 1;
    var count int64;
    set count subtract(n 1);
    repeat i count
        var next int64;
        set next add(prev curr);
        set prev curr;
        set curr next;
    end
    return curr;
end
)--";

// Recursive fibonacci - exercises function call overhead (shallow recursion)
static std::string const code_fibonacci_recursive = R"--(
function fib_rec int64
    var n int64
begin
    if less(n 2)
        return n;
    end
    return add(fib_rec(subtract(n 1)) fib_rec(subtract(n 2)));
end
)--";

std::int64_t fib_iterative_native(std::int64_t n) {
  if (n < 2) return n;
  std::int64_t prev = 0, curr = 1;
  for (std::int64_t i = 0; i < n - 1; i++) {
    std::int64_t next = prev + curr;
    prev = curr;
    curr = next;
  }
  return curr;
}

TEST_CASE("Fibonacci iterative","[!benchmark]") {
  sanema::SanemaScriptSystem sanema_script_system{1,10};
  auto script_id = sanema_script_system.add_script(code_fibonacci);

  auto fib_10 = sanema_script_system.run_function<std::int64_t>(script_id, "fib", 0, std::int64_t{10});
  auto fib_20 = sanema_script_system.run_function<std::int64_t>(script_id, "fib", 0, std::int64_t{20});
  auto fib_30 = sanema_script_system.run_function<std::int64_t>(script_id, "fib", 0, std::int64_t{30});
  CHECK(fib_10 == 55);
  CHECK(fib_20 == 6765);
  CHECK(fib_30 == 832040);

  auto fib_id = sanema_script_system.get_function_id<std::int64_t>(script_id, "fib", std::int64_t{});
  REQUIRE(fib_id.has_value());

  BENCHMARK("Fibonacci iterative 20") {
    return sanema_script_system.run_function<std::int64_t>(script_id, *fib_id, 0, std::int64_t{20});
  };

  BENCHMARK("Fibonacci iterative 30") {
    return sanema_script_system.run_function<std::int64_t>(script_id, *fib_id, 0, std::int64_t{30});
  };

  BENCHMARK("Fibonacci iterative CPP 20") {
    return fib_iterative_native(20);
  };

  BENCHMARK("Fibonacci iterative CPP 30") {
    return fib_iterative_native(30);
  };
}

TEST_CASE("Fibonacci recursive","[!benchmark]") {
  sanema::SanemaScriptSystem sanema_script_system{1,10};
  auto script_id = sanema_script_system.add_script(code_fibonacci_recursive);

  // Test with small values that work within recursion limits
  auto fib_4 = sanema_script_system.run_function<std::int64_t>(script_id, "fib_rec", 0, std::int64_t{4});
  CHECK(fib_4 == 3);

  auto fib_id = sanema_script_system.get_function_id<std::int64_t>(script_id, "fib_rec", std::int64_t{});
  REQUIRE(fib_id.has_value());

  BENCHMARK("Fibonacci recursive 4") {
    return sanema_script_system.run_function<std::int64_t>(script_id, *fib_id, 0, std::int64_t{4});
  };

  BENCHMARK("Fibonacci recursive CPP 20") {
    return fib_native(20);
  };

  BENCHMARK("Fibonacci recursive CPP 30") {
    return fib_native(30);
  };
}

// ============================================================================
// Fibonacci with optimization on/off comparison
// ============================================================================

TEST_CASE("Fibonacci optimized vs unoptimized","[!benchmark]") {
  // Unoptimized
  sanema::SanemaScriptSystem system_unopt{1,10};
  auto script_unopt = system_unopt.add_script(code_fibonacci);

  // Optimized
  sanema::SanemaScriptSystem system_opt{1,10};
  system_opt.get_optimizer().enable_pass("all");
  auto script_opt = system_opt.add_script(code_fibonacci);

  // Verify both produce same result
  auto result_unopt = system_unopt.run_function<std::int64_t>(script_unopt, "fib", 0, std::int64_t{20});
  auto result_opt = system_opt.run_function<std::int64_t>(script_opt, "fib", 0, std::int64_t{20});
  CHECK(result_unopt == 6765);
  CHECK(result_opt == 6765);

  auto fib_id_unopt = system_unopt.get_function_id<std::int64_t>(script_unopt, "fib", std::int64_t{});
  auto fib_id_opt = system_opt.get_function_id<std::int64_t>(script_opt, "fib", std::int64_t{});
  REQUIRE(fib_id_unopt.has_value());
  REQUIRE(fib_id_opt.has_value());

  BENCHMARK("Fibonacci iterative 20 - unoptimized") {
    return system_unopt.run_function<std::int64_t>(script_unopt, *fib_id_unopt, 0, std::int64_t{20});
  };

  BENCHMARK("Fibonacci iterative 20 - optimized") {
    return system_opt.run_function<std::int64_t>(script_opt, *fib_id_opt, 0, std::int64_t{20});
  };

  BENCHMARK("Fibonacci iterative 20 - native C++") {
    return fib_iterative_native(20);
  };
}

// ============================================================================
// Arithmetic-heavy benchmark (tests constant folding impact)
// ============================================================================

static std::string const code_arithmetic = R"--(
function compute int64
begin
    var result int64;
    set result multiply(add(multiply(add(multiply(add(multiply(add(multiply(add(1 2) 3) 4) 5) 6) 7) 8) 9) 10) 11);
    set result add(result multiply(add(multiply(add(multiply(add(multiply(add(multiply(add(11 12) 13) 14) 15) 16) 17) 18) 19) 20) 21));
    set result add(result multiply(add(multiply(add(multiply(add(multiply(add(multiply(add(21 22) 23) 24) 25) 26) 27) 28) 29) 30) 31));
    set result add(result multiply(add(multiply(add(multiply(add(multiply(add(multiply(add(31 32) 33) 34) 35) 36) 37) 38) 39) 40) 41));
    return result;
end
)--";

TEST_CASE("Arithmetic heavy","[!benchmark]") {
  // Unoptimized
  sanema::SanemaScriptSystem system_unopt{1,10};
  auto script_unopt = system_unopt.add_script(code_arithmetic);

  // Optimized
  sanema::SanemaScriptSystem system_opt{1,10};
  system_opt.get_optimizer().enable_pass("all");
  auto script_opt = system_opt.add_script(code_arithmetic);

  // Verify correctness
  auto result_unopt = system_unopt.run_function<std::int64_t>(script_unopt, "compute", 0);
  auto result_opt = system_opt.run_function<std::int64_t>(script_opt, "compute", 0);
  auto expected = cpp_arithmetic_heavy();
  CHECK(result_unopt == expected);
  CHECK(result_opt == expected);

  auto func_id_unopt = system_unopt.get_function_id<std::int64_t>(script_unopt, "compute");
  auto func_id_opt = system_opt.get_function_id<std::int64_t>(script_opt, "compute");
  REQUIRE(func_id_unopt.has_value());
  REQUIRE(func_id_opt.has_value());

  BENCHMARK("Arithmetic heavy - unoptimized") {
    return system_unopt.run_function<std::int64_t>(script_unopt, *func_id_unopt, 0);
  };

  BENCHMARK("Arithmetic heavy - optimized") {
    return system_opt.run_function<std::int64_t>(script_opt, *func_id_opt, 0);
  };

  BENCHMARK("Arithmetic heavy - native C++") {
    return cpp_arithmetic_heavy();
  };
}

// ============================================================================
// Loop-heavy benchmark (tests dead code / peephole impact)
// ============================================================================

static std::string const code_loop = R"--(
function loop_sum int64
begin
    var accumulator int64;
    set accumulator 0;
    repeat i 1000
        set accumulator add(accumulator i);
    end
    return accumulator;
end
)--";

TEST_CASE("Loop heavy","[!benchmark]") {
  // Unoptimized
  sanema::SanemaScriptSystem system_unopt{1,10};
  auto script_unopt = system_unopt.add_script(code_loop);

  // Optimized
  sanema::SanemaScriptSystem system_opt{1,10};
  system_opt.get_optimizer().enable_pass("all");
  auto script_opt = system_opt.add_script(code_loop);

  // Verify correctness
  auto result_unopt = system_unopt.run_function<std::int64_t>(script_unopt, "loop_sum", 0);
  auto result_opt = system_opt.run_function<std::int64_t>(script_opt, "loop_sum", 0);
  auto expected = cpp_loop_heavy();
  CHECK(result_unopt == expected);
  CHECK(result_opt == expected);

  auto func_id_unopt = system_unopt.get_function_id<std::int64_t>(script_unopt, "loop_sum");
  auto func_id_opt = system_opt.get_function_id<std::int64_t>(script_opt, "loop_sum");
  REQUIRE(func_id_unopt.has_value());
  REQUIRE(func_id_opt.has_value());

  BENCHMARK("Loop heavy - unoptimized") {
    return system_unopt.run_function<std::int64_t>(script_unopt, *func_id_unopt, 0);
  };

  BENCHMARK("Loop heavy - optimized") {
    return system_opt.run_function<std::int64_t>(script_opt, *func_id_opt, 0);
  };

  BENCHMARK("Loop heavy - native C++") {
    return cpp_loop_heavy();
  };
}

// ============================================================================
// Branch-heavy benchmark (tests dead code elimination)
// ============================================================================

static std::string const code_branch = R"--(
function classify int64
    var value int64
begin
    var result int64;
    if greater(value 50)
        if greater(value 75)
            if greater(value 90)
                set result 4;
            else
                set result 3;
            end
        else
            if greater(value 60)
                set result 2;
            else
                set result 1;
            end
        end
    else
        if greater(value 25)
            if greater(value 40)
                set result subtract(0 1);
            else
                set result subtract(0 2);
            end
        else
            if greater(value 10)
                set result subtract(0 3);
            else
                set result subtract(0 4);
            end
        end
    end
    return result;
end

function run_branches int64
begin
    var total int64;
    set total 0;
    repeat i 100
        set total add(total classify(i));
    end
    return total;
end
)--";

TEST_CASE("Branch heavy","[!benchmark]") {
  // Unoptimized
  sanema::SanemaScriptSystem system_unopt{1,10};
  auto script_unopt = system_unopt.add_script(code_branch);

  // Optimized
  sanema::SanemaScriptSystem system_opt{1,10};
  system_opt.get_optimizer().enable_pass("all");
  auto script_opt = system_opt.add_script(code_branch);

  // Verify both produce same result
  auto result_unopt = system_unopt.run_function<std::int64_t>(script_unopt, "run_branches", 0);
  auto result_opt = system_opt.run_function<std::int64_t>(script_opt, "run_branches", 0);
  CHECK(result_unopt == result_opt);

  auto func_id_unopt = system_unopt.get_function_id<std::int64_t>(script_unopt, "run_branches");
  auto func_id_opt = system_opt.get_function_id<std::int64_t>(script_opt, "run_branches");
  REQUIRE(func_id_unopt.has_value());
  REQUIRE(func_id_opt.has_value());

  // Also verify the native equivalent matches
  std::int64_t native_total = 0;
  for (std::int64_t i = 0; i < 100; i++) {
    native_total += cpp_branch_heavy(i);
  }
  CHECK(result_unopt == native_total);

  BENCHMARK("Branch heavy - unoptimized") {
    return system_unopt.run_function<std::int64_t>(script_unopt, *func_id_unopt, 0);
  };

  BENCHMARK("Branch heavy - optimized") {
    return system_opt.run_function<std::int64_t>(script_opt, *func_id_opt, 0);
  };

  BENCHMARK("Branch heavy - native C++") {
    std::int64_t total = 0;
    for (std::int64_t i = 0; i < 100; i++) {
      total += cpp_branch_heavy(i);
    }
    return total;
  };
}

// ============================================================================
// String operations benchmark
// ============================================================================

static std::string const code_string = R"--(
function string_work string
begin
    var result string;
    set result "hello";
    set result concat(result " world");
    set result concat(result " from");
    set result concat(result " sanema");
    set result concat(result " scripting");
    set result concat(result " language");
    set result concat(result " benchmark");
    set result concat(result " test");
    var check bool;
    set check ends_with(result "test");
    return result;
end
)--";

TEST_CASE("String operations","[!benchmark]") {
  // Unoptimized
  sanema::SanemaScriptSystem system_unopt{1,10};
  auto script_unopt = system_unopt.add_script(code_string);

  // Optimized
  sanema::SanemaScriptSystem system_opt{1,10};
  system_opt.get_optimizer().enable_pass("all");
  auto script_opt = system_opt.add_script(code_string);

  // Verify correctness
  auto result_unopt = system_unopt.run_function<std::string>(script_unopt, "string_work", 0);
  auto result_opt = system_opt.run_function<std::string>(script_opt, "string_work", 0);
  CHECK(result_unopt == "hello world from sanema scripting language benchmark test");
  CHECK(result_opt == "hello world from sanema scripting language benchmark test");

  auto func_id_unopt = system_unopt.get_function_id<std::string>(script_unopt, "string_work");
  auto func_id_opt = system_opt.get_function_id<std::string>(script_opt, "string_work");
  REQUIRE(func_id_unopt.has_value());
  REQUIRE(func_id_opt.has_value());

  BENCHMARK("String operations - unoptimized") {
    return system_unopt.run_function<std::string>(script_unopt, *func_id_unopt, 0);
  };

  BENCHMARK("String operations - optimized") {
    return system_opt.run_function<std::string>(script_opt, *func_id_opt, 0);
  };

  BENCHMARK("String operations - native C++") {
    std::string result = "hello";
    result += " world";
    result += " from";
    result += " sanema";
    result += " scripting";
    result += " language";
    result += " benchmark";
    result += " test";
    bool check = result.ends_with("test");
    return result;
  };
}

// ============================================================================
// Mixed realistic benchmark (game-logic-like: conditionals + math + calls)
// ============================================================================

static std::string const code_mixed = R"--(
function clamp int64
    var value int64
    var min_val int64
    var max_val int64
begin
    if less(value min_val)
        return min_val;
    end
    if greater(value max_val)
        return max_val;
    end
    return value;
end

function compute_damage int64
    var health int64
    var armor int64
    var damage int64
begin
    var effective_damage int64;
    set effective_damage subtract(damage divide(armor 2));
    set effective_damage clamp(effective_damage 1 999);
    var new_health int64;
    set new_health subtract(health effective_damage);
    set new_health clamp(new_health 0 9999);
    var score int64;
    set score 0;
    repeat i 100
        set score add(score add(new_health i));
    end
    return score;
end
)--";

TEST_CASE("Mixed realistic","[!benchmark]") {
  // Unoptimized
  sanema::SanemaScriptSystem system_unopt{1,10};
  auto script_unopt = system_unopt.add_script(code_mixed);

  // Optimized
  sanema::SanemaScriptSystem system_opt{1,10};
  system_opt.get_optimizer().enable_pass("all");
  auto script_opt = system_opt.add_script(code_mixed);

  // Verify both produce same result
  auto result_unopt = system_unopt.run_function<std::int64_t>(
    script_unopt, "compute_damage", 0, std::int64_t{100}, std::int64_t{50}, std::int64_t{30});
  auto result_opt = system_opt.run_function<std::int64_t>(
    script_opt, "compute_damage", 0, std::int64_t{100}, std::int64_t{50}, std::int64_t{30});
  CHECK(result_unopt == result_opt);

  // Verify against native
  auto expected = cpp_mixed_realistic(100, 50, 30);
  CHECK(result_unopt == expected);

  auto func_id_unopt = system_unopt.get_function_id<std::int64_t>(
    script_unopt, "compute_damage", std::int64_t{}, std::int64_t{}, std::int64_t{});
  auto func_id_opt = system_opt.get_function_id<std::int64_t>(
    script_opt, "compute_damage", std::int64_t{}, std::int64_t{}, std::int64_t{});
  REQUIRE(func_id_unopt.has_value());
  REQUIRE(func_id_opt.has_value());

  BENCHMARK("Mixed realistic - unoptimized") {
    return system_unopt.run_function<std::int64_t>(
      script_unopt, *func_id_unopt, 0, std::int64_t{100}, std::int64_t{50}, std::int64_t{30});
  };

  BENCHMARK("Mixed realistic - optimized") {
    return system_opt.run_function<std::int64_t>(
      script_opt, *func_id_opt, 0, std::int64_t{100}, std::int64_t{50}, std::int64_t{30});
  };

  BENCHMARK("Mixed realistic - native C++") {
    return cpp_mixed_realistic(100, 50, 30);
  };
}
