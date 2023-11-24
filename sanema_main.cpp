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

#include "binding/BindingCollection.h"
#include "built-in/print.h"
#include "parsing/SanemaParser.hpp"
#include "SanemaScriptSystem.h"

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
          return"struct";
        },
        [](sanema::Void &a_struct) {
          return "void";
        }

       );
}

void print_function_call(sanema::FunctionCall &function_call, int level = 0) {
  std::string tab;
  for (int i = 0; i < level * 3; i++) {
    tab+=" ";
  }
  std::cout << tab<<"Calling Function " << function_call.identifier;
  std::cout << tab<<" With arguments: \n";
  for (auto &argument: function_call.arguments) {
    match(argument.expression,
          [&tab](sanema::VariableEvaluation &variable_evaluation) {
            std::cout << "   "<<tab<<variable_evaluation.identifier << "\n";
          },
          [&tab](sanema::Literal &literal) {
            std::cout <<"   "<<tab<< "Literal\n";
          },
          [level,&tab](sanema::FunctionCall &function_call_argument) {
            print_function_call(function_call_argument,
                                level + 1);
          }
         );
  }
}

void print_block_of_code(sanema::BlockOfCode block_of_code){
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
          [](sanema::DeclareVariable &expression) {
            std::cout << "Declaring variable: " << expression.identifier << " of type ";
            print_type(expression.type_identifier);
            std::cout << "\n";
          },
          [](sanema::DefineFunction &expression) {
            std::cout << "Declaring Function: " << expression.identifier << " with type: "<<serialize_type(expression.type) <<" and  parameters \n";
            for (auto &parameter: expression.parameters) {
              std::cout << "modifier:"<< sanema::serialize_modifier(parameter.modifier)<<" identifier: "<< parameter.identifier << " type: ";
              if(parameter.type.has_value()) {
                print_type(parameter.type.value());
              }else{
                std::cout<<" NO_TYPE ";
              }
            }
            std::cout<<"\n{\n";
            print_block_of_code(expression.body);
            std::cout<<"}\n";
          },
          [](sanema::FunctionCall &expression) {
            print_function_call(expression);

          },
          [](sanema::BlockOfCode &expression) {
            std::cout<<"{\n";
            print_block_of_code(expression);
            std::cout<<"}\n";
          }
         );

  }
}
int main(int argc, char *argv[]) {
  sanema::SanemaParser parser;
  std::ifstream f{"res/test.san", std::ios::in};
  if(!f.is_open()){
    std::cerr<<"file not found \n";
  }
  try {
    sanema::SanemaScriptSystem scriptSystem;
    auto id = scriptSystem.add_script(f);
    scriptSystem.run_script(id);
  }catch (std::runtime_error &e){
    std::cout<<e.what()<<"\n";
  }




}