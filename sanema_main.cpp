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
    sanema::SanemaScriptSystem scriptSystem{1, 10};
    scriptSystem.add_pod_type<vec3>("vec3")
      ->with_field<float>("x",
                   offsetof(vec3,x))
      ->with_field<float>("y",
                   offsetof(vec3,y))
      ->with_field<float>("z",
                  offsetof(vec3,z));
    scriptSystem.add_function("generate_float",generate_float);
    auto id = scriptSystem.add_script(f);

    std::chrono::high_resolution_clock clock;
    std::cout << "\n started\n";
    auto first = clock.now();
    scriptSystem.run_script(id,0);
    auto second = clock.now();
    std::cout << "\n duration:" << std::chrono::duration_cast<std::chrono::milliseconds>(second - first) << "\n";
    bool return_value;
    scriptSystem.get_return_value(return_value,0);
    std::cout<<"result:"<<(return_value?"true":"false")<<"\n";

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

  sanema::SanemaScriptSystem script_system{1,100};
  auto script_id=script_system.add_script(code);
  int64_t  value=5;

  script_system.run_function<int64_t >(script_id,"return_function",0,value);
  } catch (std::runtime_error &e) {
    std::cout << e.what() << "\n";
  }


}