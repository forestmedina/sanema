//
// Created by fores on 9/18/2023.
//

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <built-in/built_in_functions.h>
#include <built-in/strings/strings.h>
#include <compiler/ByteCodeCompiler.h>
#include <parsing/ValidationStage.h>
#include <util/lambda_visitor.hpp>
#include <vm/VM.h>
#include <lua.hpp>
#include "binding/BindingCollection.h"
#include "built-in/print.h"
#include "parsing/SanemaParser.hpp"
#include "SanemaScriptSystem.h"
#include <chrono>
#include <angelscript.h>

void print_type(sanema::CompleteType &type) {
  match(type,
        [](sanema::Integer &integer) {
          std::cout << "integer " << integer.size;

        },
        [](sanema::Float &a_float) {
          std::cout << "float";
        },
        [](sanema::Double &a_double) {
          std::cout << "double";
        },
        [](sanema::String &a_string) {
          std::cout << "string";
        },
        [](sanema::Boolean &a_boolean) {
          std::cout << "boolean";
        },
        [](sanema::UserDefined &a_struct) {
          std::cout << "struct";
        },
        [](sanema::Void &a_struct) {
          std::cout << "void";
        }

       );
}

std::string serialize_type(sanema::CompleteType &type) {
  return match(type,
               [](sanema::Integer &integer) {
                 return "integer ";
               },
               [](sanema::Float &a_float) {
                 return "float";
               },
               [](sanema::Double &a_double) {
                 return "double";
               },
               [](sanema::String &a_string) {
                 return "string";
               },
               [](sanema::Boolean &a_boolean) {
                 return "boolean";
               },
               [](sanema::UserDefined &a_struct) {
                 return "struct";
               },
               [](sanema::Void &a_struct) {
                 return "void";
               }

              );
}

void print_function_call(sanema::FunctionCall &function_call, int level = 0) {
  std::string tab;
  for (int i = 0; i < level * 3; i++) {
    tab += " ";
  }
  std::cout << tab << "Calling Function " << function_call.identifier;
  std::cout << tab << " With arguments: \n";
  for (auto &argument: function_call.arguments) {
    match(argument.expression,
          [&tab](sanema::VariableEvaluation &variable_evaluation) {
            std::cout << "   " << tab << variable_evaluation.identifier << "\n";
          },
          [&tab](sanema::Literal &literal) {
            std::cout << "   " << tab << "Literal\n";
          },
          [level, &tab](sanema::FunctionCall &function_call_argument) {
            print_function_call(function_call_argument,
                                level + 1);
          }
         );
  }
}

void print_block_of_code(sanema::BlockOfCode block_of_code) {
  for (auto &expresion: block_of_code.instructions) {
    match(expresion.instruction_sum,
          [](sanema::DefineStruct &expression) {
            if (!expression.user_type.has_value()) return;
            std::cout << "define struct: " << expression.user_type.value().type_id.identifier << "\n";
            for (auto field: expression.user_type.value().fields) {
              std::cout << "field : " << field.identifier << " of type ";
              if (field.type.has_value()) {
                print_type(field.type.value());
              }
              std::cout << "\n";
            }

          },
          [](sanema::IfStatement &if_statement) {
          },
          [](sanema::DeclareVariable &expression) {
            std::cout << "Declaring variable: " << expression.identifier << " of type ";
            print_type(expression.type_identifier);
            std::cout << "\n";
          },
          [](sanema::DefineFunction &expression) {
            std::cout << "Declaring Function: " << expression.identifier << " with type: "
                      << serialize_type(expression.type) << " and  parameters \n";
            for (auto &parameter: expression.parameters) {
              std::cout << "modifier:" << sanema::serialize_modifier(parameter.modifier) << " identifier: "
                        << parameter.identifier << " type: ";
              if (parameter.type.has_value()) {
                print_type(parameter.type.value());
              } else {
                std::cout << " NO_TYPE ";
              }
            }
            std::cout << "\n{\n";
            print_block_of_code(expression.body);
            std::cout << "}\n";
          },
          [](sanema::FunctionCall &expression) {
            print_function_call(expression);

          },
          [](sanema::BlockOfCode &expression) {
            std::cout << "{\n";
            print_block_of_code(expression);
            std::cout << "}\n";
          }
         );

  }
}

struct vec3 {
  float x;
  float y;
  float z;
};

std::uint64_t fib(std::uint64_t n) {
  if (n < 2) {
    return n;
  } else {
    return fib(n - 1) +
           fib(n - 2);
  }
}

float generate_float(float n){
  return 10.0f;
}
int main(int argc, char *argv[]) {

  sanema::SanemaParser parser;
  std::ifstream f{"res/test.san", std::ios::in};
  if (!f.is_open()) {
    std::cerr << "file not found \n";
  }
  try {
    sanema::SanemaScriptSystem scriptSystem;
    scriptSystem.add_type<vec3>("vec3")
      ->with_field("x",
                   &vec3::x)
      ->with_field("y",
                   &vec3::y)
      ->with_field("z",
                   &vec3::z);
    scriptSystem.add_function("generate_float",generate_float);
    auto id = scriptSystem.add_script(f);

    std::chrono::high_resolution_clock clock;
    std::cout << "\n started\n";
    auto first = clock.now();
    scriptSystem.run_script(id);
    auto second = clock.now();
    std::cout << "\n duration:" << std::chrono::duration_cast<std::chrono::milliseconds>(second - first) << "\n";
    bool return_value;
    scriptSystem.get_return_value(return_value);
    std::cout<<"result:"<<(return_value?"true":"false")<<"\n";
//    asIScriptEngine *engine = asCreateScriptEngine();
//    const char *fibonacciScript =
//      "int fibonacci(int n) {"
//      "    if (n <= 1) return n;"
//      "    return fibonacci(n - 1) + fibonacci(n - 2);"
//      "}";
//
//    asIScriptModule *module = engine->GetModule("FibModule",
//                                                asGM_ALWAYS_CREATE);
//    module->AddScriptSection("fibScript",
//                             fibonacciScript);
//    module->Build();
//    asIScriptFunction *func = module->GetFunctionByDecl("int fibonacci(int)");
//    asIScriptContext *context = engine->CreateContext();
//    context->Prepare(func);
//    context->SetArgDWord(0,
//                         std::int64_t(35)); // Change the parameter here for different Fibonacci numbers
//    std::cout << "\n started angel\n";
//    first = clock.now();
//    context->Execute();
//    second = clock.now();
//    std::cout << "\n duration:" << std::chrono::duration_cast<std::chrono::milliseconds>(second - first) << "\n";
//
//     lua_State* L = luaL_newstate();
//    luaL_openlibs(L);
//
//    const char* fibonacciLuaScript =
//    "function fibonacci(n) "
//    "    if n <= 1 then return n end "
//    "    return fibonacci(n - 1) + fibonacci(n - 2) "
//    "end";
//    // Load the Fibonacci script into Lua
//    luaL_dostring(L, fibonacciLuaScript);
// std::cout << "\n started lua\n";
//    first = clock.now();
//     luaL_loadstring(L, "result = fibonacci(35)"); // Change the parameter here for different Fibonacci numbers
//    lua_pcall(L, 0, 0, 0);
//    second = clock.now();
//    std::cout << "\n duration:" << std::chrono::duration_cast<std::chrono::milliseconds>(second - first) << "\n";
//    // Start measuring time
//    first =clock.now();
//    auto value=fib(35);
//    second =clock.now();
//    std::cout<<"\n cpp duration:"<<std::chrono::duration_cast<std::chrono::milliseconds>(second-first)<<"\n";
//    std::cout<<value<<"\n";
  } catch (std::runtime_error &e) {
    std::cout << e.what() << "\n";
  }


}