//
// Created by fores on 23/3/2022.
//

#include "SanemaParser.hpp"
#include <set>
#include <format>
#include <algorithm>
#include "expressions.h"
#include "util/lambda_visitor.hpp"
#include <iostream>
#include <istream>
#include <stack>
#include <boost/lexical_cast.hpp>

enum class ParserState {
  DefininVariable,
  DefiningFunction,

} parser_state;
enum class VariableParsingState {
  ParsingIdentifier,
  ParsingType
};


struct StateNone {
};


char read_character = ' ';


sanema::BlockOfCode sanema::SanemaParser::parse(const std::vector<sanema::Token> &tokens) {
  struct Context {
    BlockOfCode current_block{};
    std::optional<Instruction> instruction;
    std::stack<sanema::FunctionCall> function_call_stack{};
  };
  std::stack<Context> context_stack;
  Context current_context;

  auto stack_context = [&current_context, &context_stack]() {
    context_stack.emplace(current_context);
    current_context = {};
  };
  for (auto token_it = tokens.begin(); token_it != tokens.end(); token_it++) {
    auto token = *token_it;
    auto next_token_it = (token_it + 1);
    std::optional<sanema::Token> next_token =
      next_token_it != tokens.end() ? std::optional<sanema::Token>{*next_token_it} : std::optional<sanema::Token>{};
    if (!current_context.instruction.has_value()) {

      if (token.token == variable_declaring_word) {
        current_context.instruction = DeclareVariable{};
      } else if (token.token == function_declaring_word) {
        current_context.instruction = DefineFunction{};
      } else if (token.token == struct_declaring_word) {
        current_context.instruction = DefineStruct{};
      } else if (token.token == "{") {
        stack_context();
      } else if (token.token == "}") {
        auto aux_context = current_context;
        current_context = context_stack.top();
        context_stack.pop();
        if (current_context.instruction.has_value()) {
          match(current_context.instruction.value(),
                [&](DefineFunction &define_function) {
                  std::cout << "block ffinished define function\n";
                  define_function.body = aux_context.current_block;
                  current_context.current_block.instructions.emplace_back(define_function);
                  current_context.instruction = {};
                },
                [&](auto &ignore) {
                  std::cout << "block ffinished ignore\n";
                  current_context.current_block.instructions.emplace_back(aux_context.current_block);
                }
               );
        } else {
          current_context.current_block.instructions.emplace_back(aux_context.current_block);
        }
      } else if (!is_delimiter(token.token)) {
        current_context.instruction = FunctionCall{token.token};
      }
    } else {
      match(current_context.instruction.value(),
            [&](DefineStruct &define_struct) {
              switch (define_struct.state) {
                case DefineStruct::IDENTIFIER:
                  define_struct.user_type = Struct(TypeId(token.token));
                  define_struct.state = DefineStruct::FIELD_IDENTIFIER;
                  break;
                case DefineStruct::FIELD_TYPE:

                  define_struct.user_type->fields.back().type = parse_type(token.token);
                  define_struct.state = DefineStruct::FIELD_COMPLETE;
                  break;
                case DefineStruct::FIELD_IDENTIFIER:
                  define_struct.user_type->fields.emplace_back();
                  define_struct.user_type->fields.back().identifier = token.token;
                  define_struct.state = DefineStruct::FIELD_TYPE;
                  break;
                case DefineStruct::FIELD_COMPLETE:
                  if (token.token == ",") {
                    define_struct.state = DefineStruct::FIELD_IDENTIFIER;
                  } else if (token.token == ";") {
                    current_context.current_block.instructions.emplace_back(current_context.instruction.value());
                    current_context.instruction = {};
                  }
                  break;
              }
            },
            [&](DefineFunction &define_function) {
              switch (define_function.state) {
                case DefineFunction::IDENTIFIER: {
                  define_function.identifier = token.token;
                  std::cout << "Function identifier: " << token.token << "\n";
                  define_function.state = DefineFunction::FUNCTION_TYPE;

                }
                  break;
                case DefineFunction::FUNCTION_TYPE: {
                  define_function.type = parse_type(token.token).value();
                  define_function.state = DefineFunction::PARAMETER_MODIFIER;

                }
                  break;
                case DefineFunction::PARAMETER_MODIFIER: {
                  std::cout << "function modifier token -" << token.token << "-\n";
                  if (token.token == std::string("") + code_block_begin) {
                    std::cout << "stacking block for function body\n";
                    define_function.state = DefineFunction::FUNCTION_BODY;
                    stack_context();
                  } else {
                    define_function.parameter.emplace_back();
                    define_function.parameter.back().modifier = parse_modifier(token.token);
                    define_function.state = DefineFunction::PARAMETER_IDENTIFIER;
                  }

                }
                  break;
                case DefineFunction::PARAMETER_IDENTIFIER: {
                  define_function.parameter.back().identifier = token.token;
                  define_function.state = DefineFunction::PARAMETER_TYPE;
                }
                  break;
                case DefineFunction::PARAMETER_TYPE: {
                  std::cout << "Function parameter type: " << token.token << "\n";
                  define_function.parameter.back().type = parse_type(token.token);
                  define_function.state = DefineFunction::PARAMETER_MODIFIER;
                }
                  break;

              }
            },
            [&](BlockOfCode &block_of_code) {


            },
            [&](DeclareVariable &declare_variable) {
              switch (declare_variable.state) {
                case DeclareVariable::DeclareVariableState::TYPE:
                  declare_variable.type_identifier = parse_type(token.token).value();
                  declare_variable.state = DeclareVariable::DeclareVariableState::COMPLETE;
                  break;
                case DeclareVariable::DeclareVariableState::IDENTIFIER:
                  declare_variable.identifier = token.token;
                  declare_variable.state = DeclareVariable::DeclareVariableState::TYPE;
                  break;
                case DeclareVariable::DeclareVariableState::COMPLETE:
                  if (token.token == ";") {
                    current_context.current_block.instructions.emplace_back(current_context.instruction.value());
                    current_context.instruction = {};
                  }
                  break;
              }
            },
            [&](FunctionCall &function_call) {
              switch (function_call.state) {
                case FunctionCall::ARGUMENT_EXPRESION:
                  if (is_literal(token.token)) {
                    function_call.arguments.emplace_back(FunctionArgument{get_literal_from_string(token.token)});
                  } else if (token.token == "(") {
                    //ignore it
                  } else if (token.token == ")" || token.token == ";") {
                    if (current_context.function_call_stack.empty()) {
                      current_context.current_block.instructions.emplace_back(function_call);
                      current_context.instruction = {};
                    } else {
                      std::cout << "unstacking function\n";
                      FunctionCall previous_function_call = current_context.function_call_stack.top();
                      current_context.function_call_stack.pop();
                      previous_function_call.arguments.emplace_back(FunctionArgument{function_call});
                      current_context.instruction = previous_function_call;
                    }
                  } else {
                    auto next_token_it = std::next(token_it);
                    if (next_token_it != tokens.end()) {
                      auto &next_token = *next_token_it;
                      if (next_token.token == "(") {
                        std::cout << "stacking function" << function_call.identifier << "\n";
                        current_context.function_call_stack.emplace(function_call);
                        auto new_function_call = FunctionCall{};
                        new_function_call.identifier = token.token;
                        current_context.instruction = new_function_call;
                      } else {
                        function_call.arguments.emplace_back(FunctionArgument{VariableEvaluation{token.token}});
                      }

                    }
                  }
                  break;
              }
            }
           );
    }
  }

  return current_context.current_block;
}

std::vector<sanema::Token> sanema::SanemaParser::tokenize(std::istream &text) {


  std::optional<Token> token{Token{"", 0, 0}};
  std::vector<Token> tokens;
  long line_number = 0;
  long column_number = 0;
  while (!text.eof()) {
    text.get(read_character);
    if (delimiters.count(read_character) > 0) {
      if (token.has_value()) {
        tokens.push_back(*token);
        token = {};
      }
      if (separators.count(read_character)) {
        continue;
      }
      tokens.emplace_back(std::string("") + read_character,
                          line_number,
                          column_number);
    } else if (line_ending_character.count(read_character) > 0) {
      if (token.has_value()) {
        tokens.push_back(*token);
        token = {};
      }
      line_number++;
      column_number = 0;
    } else if (ignored_characters.count(read_character) > 0) {
      //ignore
    } else {
      if (!token.has_value()) {
        token = Token{"", (int) line_number + 1, column_number};
      }
      token->token += read_character;
    }
    column_number++;

  }
  return tokens;
}

sanema::SanemaParser::SanemaParser() {
  delimiters.insert(separators.begin(),
                    separators.end());
  delimiters.insert(function_delimiters.begin(),
                    function_delimiters.end());
  delimiters.insert(terminators.begin(),
                    terminators.end());
  delimiters.insert(string_delimiters.begin(),
                    string_delimiters.end());
  delimiters.insert(code_block_delimiters.begin(),
                    code_block_delimiters.end());
  delimiters.insert(field_separator.begin(),
                    field_separator.end());
}

bool sanema::SanemaParser::is_delimiter(std::string const &string) {
  for (auto delimiter: delimiters) {
    if ((std::string("") + delimiter) == string) {
      return true;
    }
  }
  return false;
}

bool sanema::SanemaParser::is_literal(const std::string &string) {
  if (string.empty()) return false;
  std::set<char> literals_characters{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '"'};
  return literals_characters.count(string[0]) > 0;
}


sanema::Token::Token(std::string &&token, long lineNumber, int columnNumber)
  : token(token), line_number(lineNumber), column_number(columnNumber) {}

sanema::Literal sanema::SanemaParser::get_literal_from_string(std::string token) {
  try {
    auto value = boost::lexical_cast<std::int64_t>(token);
    if (value >= std::numeric_limits<std::uint8_t>::min() && value <= std::numeric_limits<std::uint8_t>::max()) {
      return LiteralSInt8(value);
    }
    if (value >= std::numeric_limits<std::uint16_t>::min() && value <= std::numeric_limits<std::uint16_t>::max()) {
      return LiteralSInt16(value);
    }
    if (value >= std::numeric_limits<std::uint32_t>::min() && value <= std::numeric_limits<std::uint32_t>::max()) {
      return LiteralSInt32(value);
    }
    if (value >= std::numeric_limits<std::uint64_t>::min() && value <= std::numeric_limits<std::uint64_t>::max()) {
      return LiteralSInt64(value);
    }
  } catch (boost::exception &ex) {


  }

  try {
    if (token.ends_with("d")) {
      auto double_value = boost::lexical_cast<std::double_t>(token.replace(token.end() - 1,
                                                                           token.end(),
                                                                           ""));
      return LiteralDouble(double_value);
    }
  } catch (boost::exception &ex) {

  }

  try {
    auto float_token = token;
    if (float_token.ends_with("f")) {
      float_token.replace(float_token.end() - 1,
                          float_token.end(),
                          "");
    }

    auto float_value = boost::lexical_cast<std::float_t>(float_token);
    return LiteralFloat(float_value);
  } catch (boost::exception &ex) {

  }
  return LiteralSInt64{atoi(token.c_str())};
}
