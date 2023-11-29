#include <catch2/catch_all.hpp>
#include <parsing/SanemaParser.hpp>
#include <compiler/ByteCodeCompiler.h>
#include <vm/VM.h>
#include <sstream>
#include <test/helpers.h>

std::uint64_t fib(std::uint64_t n) {
  if (n == 0) {
    return n;
  } else {
    if (n == 1) {
      return n;
    } else {
      return fib(n - 1) +
             fib(n - 2);
    }
  }
}

TEST_CASE("Fibonacci","[!benchmark]") {

  std::string code_fibonacci_20(R"--(
function fib int64
    var n int64 {
        if equal( n 0)
            return n;
        else
            if equal (n 1)
              return n;
            else
              return  add(fib(subtract(n  1))  fib(subtract(n  2)));
            end;
        end
    };

var result int64;
fib (20);
)--");

  std::string code_fibonacci_90(R"--(
function fib int64
    var n int64 {
        if equal( n 0)
            return n;
        else
            if equal (n 1)
              return n;
            else
              return  add(fib(subtract(n  1))  fib(subtract(n  2)));
            end;
        end
    };

var result int64;
fib (30);
)--");
  sanema::SanemaScriptSystem sanema_script_system;
  auto script_20_id = sanema_script_system.add_script(code_fibonacci_20);
  auto script_90_id = sanema_script_system.add_script(code_fibonacci_90);
  sanema_script_system.run_script(script_20_id);
  std::uint64_t fibonacci_20_result;
  sanema_script_system.run_script(script_20_id);
  sanema_script_system.get_return_value(fibonacci_20_result);
  std::uint64_t fibonacci_90_result;
  sanema_script_system.run_script(script_90_id);
  sanema_script_system.get_return_value(fibonacci_90_result);
  CHECK(fibonacci_90_result == 832040);
  // some more asserts..
  CHECK(fibonacci_20_result == 6765);
  // some more asserts..

  // now let's benchmark:
  BENCHMARK("Fibonacci 20") {
                              sanema_script_system.run_script(script_20_id);
                            };

  BENCHMARK("Fibonacci 90") {
                              sanema_script_system.run_script(script_90_id);
                            };

  BENCHMARK("Fibonacci CPP 20") {
                              return fib(20);
                            };

  BENCHMARK("Fibonacci CPP 90") {
                              return fib(30);
                            };
}