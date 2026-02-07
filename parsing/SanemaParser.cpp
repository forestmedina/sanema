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
#include <boost/algorithm/string.hpp>


enum class VariableParsingState {
  ParsingIdentifier,
  ParsingType
};


struct StateNone {
};





sanema::BlockOfCode sanema::SanemaParser::parse(const std::vector<sanema::Token> &tokens) {
  using NestedExpression = std::variant<FunctionCall, IfStatement,ReturnStatement>;
  struct Context {
    BlockOfCode current_block{};

    std::optional<Instruction> instruction;
    std::stack<NestedExpression> function_call_stack{};
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
      } else if (token.token == if_word) {
        current_context.instruction = IfStatement{};
      }else if (token.token == for_loop_word){
        current_context.instruction=ForStatement{};
      }else if (token.token == return_word) {
        std::cout<<" return statement parse begin:\n";
        current_context.instruction = ReturnStatement{};
      } else if (token.token == code_block_begin) {
        stack_context();
      } else if (token.token == code_block_end || token.token == if_else_word || token.token == if_ending_word) {
        auto aux_context = current_context;
        current_context = context_stack.top();
        context_stack.pop();

        if (current_context.instruction.has_value()) {
          match(current_context.instruction.value(),
                [&](DefineFunction &define_function) {
//                  std::cout << "block ffinished define function\n";
                  define_function.body = aux_context.current_block;
                  current_context.current_block.instructions.emplace_back(define_function);
                  current_context.instruction = {};
                },
                [&](IfStatement &if_statement) {
//                  std::cout << "block ffinished define function\n";
                  switch (if_statement.state) {
                    case IfStatement::IfStatementState::TRUE_PATH:
                      if_statement.true_path = aux_context.current_block;
                      if (token.token == if_else_word) {
                        if_statement.state = IfStatement::IfStatementState::FALSE_PATH;
                        stack_context();
                      } else {
                        if_statement.false_path = BlockOfCode();
                        current_context.current_block.instructions.emplace_back(if_statement);
                        current_context.instruction = {};
                      }
                      break;
                    case IfStatement::IfStatementState::FALSE_PATH:
                      if_statement.false_path = aux_context.current_block;
                      current_context.current_block.instructions.emplace_back(if_statement);
                      current_context.instruction = {};
                      break;

                    case IfStatement::IfStatementState::EXPRESSION:
                      break;
                  }
                },
                [&](ReturnStatement &return_statement) {
//                  std::cout << "block ffinished define function\n";
                  switch (return_statement.state){
                    case ReturnStatement::ReturnStatementState::EXPRESSION:
                      // std::cout<<" finishing return statement:\n";
                      // current_context.current_block.instructions.emplace_back(return_statement);
                      // current_context.instruction = {};
                      break;
                  }
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
                  define_struct.type_id=TypeIdentifier(token.token);
                  define_struct.state = DefineStruct::FIELD_IDENTIFIER;
                  break;
                case DefineStruct::FIELD_TYPE:

                  define_struct.fields.back().type = parse_type(token.token);
                  define_struct.state = DefineStruct::FIELD_COMPLETE;
                  break;
                case DefineStruct::FIELD_IDENTIFIER:
                  define_struct.fields.emplace_back();
                  define_struct.fields.back().identifier = token.token;
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
            }, [&](ForStatement &define_struct) {

            },
            [&](IfStatement &if_statement) {
              switch (if_statement.state) {
                case IfStatement::IfStatementState::EXPRESSION:
                  if (is_literal(token.token)) {
                    if_statement.expression = get_literal_from_string(token.token);
                  } else {
                    auto next_token_it = std::next(token_it);
                    if (next_token_it != tokens.end()) {
                      auto &next_token = *next_token_it;
                      if (next_token.token == "(") {
                        current_context.function_call_stack.emplace(if_statement);
                        auto new_function_call = FunctionCall{};
                        new_function_call.identifier = token.token;
                        current_context.instruction = new_function_call;
                      } else {
                        if_statement.expression = VariableEvaluation{token.token};
                      }

                    }
                  }
                  break;
              }

            },[&](ReturnStatement &return_statement) {
              switch (return_statement.state) {
                case ReturnStatement::ReturnStatementState::EXPRESSION:
                  if(token.token==";"){
                    current_context.current_block.instructions.emplace_back(current_context.instruction.value());
                    current_context.instruction = {};
                  }else  if (is_literal(token.token)) {
                    std::cout<<" return statement parsing literal"<<token.token<<"\n";
                    return_statement.expression = get_literal_from_string(token.token);
                    current_context.current_block.instructions.emplace_back(current_context.instruction.value());
                    current_context.instruction = {};
                  } else {
                    auto next_token_it = std::next(token_it);
                    if (next_token_it != tokens.end()) {
                      auto &next_token = *next_token_it;
                      if (next_token.token == "(") {
                        current_context.function_call_stack.emplace(return_statement);
                        auto new_function_call = FunctionCall{};
                        new_function_call.identifier = token.token;
                        current_context.instruction = new_function_call;
                      }else{
                        return_statement.expression = VariableEvaluation{token.token};
                        current_context.current_block.instructions.emplace_back(current_context.instruction.value());
                        current_context.instruction = {};
                      }

                    }
                  }
                  break;
              }

            },
            [&](DefineFunction &define_function) {
              switch (define_function.state) {
                case DefineFunction::IDENTIFIER: {
                  define_function.identifier = token.token;
//                  std::cout << "Function identifier: " << token.token << "\n";
                  define_function.state = DefineFunction::FUNCTION_TYPE;

                }
                  break;
                case DefineFunction::FUNCTION_TYPE: {
                  define_function.type = parse_type(token.token).value();
                  define_function.state = DefineFunction::PARAMETER_MODIFIER;

                }
                  break;
                case DefineFunction::PARAMETER_MODIFIER: {
//                  std::cout << "function modifier token -" << token.token << "-\n";
                  if (token.token == std::string("") + code_block_begin) {
//                    std::cout << "stacking block for function body\n";
                    define_function.state = DefineFunction::FUNCTION_BODY;
                    stack_context();
                  } else {
                    define_function.parameters.emplace_back();
                    define_function.parameters.back().modifier = parse_modifier(token.token);
                    define_function.state = DefineFunction::PARAMETER_IDENTIFIER;
                  }

                }
                  break;
                case DefineFunction::PARAMETER_IDENTIFIER: {
                  define_function.parameters.back().identifier = token.token;
                  define_function.state = DefineFunction::PARAMETER_TYPE;
                }
                  break;
                case DefineFunction::PARAMETER_TYPE: {
//                  std::cout << "Function parameter type: " << token.token << "\n";
                  define_function.parameters.back().type = parse_type(token.token);
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
//                      std::cout << "unstacking function\n";
                      auto previous_instruction = current_context.function_call_stack.top();
                      match(previous_instruction,
                            [&function_call, &current_context, &stack_context](IfStatement &previous_if_statement) {
                              previous_if_statement.expression = function_call;
                               current_context.function_call_stack.pop();
                              previous_if_statement.state = IfStatement::IfStatementState::TRUE_PATH;
                              current_context.instruction = previous_if_statement;
                              stack_context();
                            },
                            [&function_call, &current_context, &stack_context](ReturnStatement &previous_return_statement) {
                             previous_return_statement.expression = function_call;
                             current_context.function_call_stack.pop();
                             current_context.instruction = previous_return_statement;
                           },
                            [&function_call, &current_context](FunctionCall &previous_function_call) {
                              current_context.function_call_stack.pop();
                              previous_function_call.arguments.emplace_back(FunctionArgument{function_call});
                              current_context.instruction = previous_function_call;
                            }
                           );


                    }
                  } else {
                    auto next_token_it = std::next(token_it);
                    if (next_token_it != tokens.end()) {
                      auto &next_token = *next_token_it;
                      if (next_token.token == "(") {
//                        std::cout << "stacking function" << function_call.identifier << "\n";
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
  bool reading_string = false;
  bool scaping_character = false;
  char read_character = ' ';
  while (text.get(read_character)) {
    if (!reading_string) {
      if (delimiters.count(read_character) > 0) {

        if (token.has_value() && !token->token.empty()) {
          tokens.push_back(*token);
          token = {};
        }
        if (separators.count(read_character)) {
          continue;
        }
        if (string_delimiters.contains(read_character)) {
          reading_string = true;
          token = Token{std::string("") + read_character, (int) line_number + 1, static_cast<int>(column_number)};
          continue;
        }
        if (!(std::string("") + read_character).empty()) {
          tokens.emplace_back(std::string("") + read_character,
                              line_number,
                              column_number);
        }

      } else if (line_ending_character.count(read_character) > 0) {
        if (token.has_value() && !token->token.empty()) {
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
    } else {
      column_number++;
      token->token += read_character;

      if (line_ending_character.contains(read_character)) {
        throw std::runtime_error("Missing closing \"");
      }
      if (string_delimiters.contains(read_character)) {
        reading_string = false;
      }

    }

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
//  delimiters.insert(code_block_delimiters.begin(),
//                    code_block_delimiters.end());
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


sanema::Token::Token(std::string &&token,
                     long
                     lineNumber, int
                     columnNumber)
  : token(token), line_number(lineNumber), column_number(columnNumber) {}

sanema::Literal sanema::SanemaParser::get_literal_from_string(std::string token) {
  try {
    auto value = boost::lexical_cast<std::int64_t>(token);

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
  if (token.starts_with("\"") && token.ends_with("\"")) {
    token.replace(0,
                  1,
                  "");
    token.replace(token.length() - 1,
                  1,
                  "");
    boost::algorithm::replace_all(token,
                                  "\\n",
                                  "\n");
    boost::algorithm::replace_all(token,
                                  "\\t",
                                  "\t");
    boost::algorithm::replace_all(token,
                                  "\\r",
                                  "\r");
    boost::algorithm::replace_all(token,
                                  "\\b",
                                  "\b");
    return LiteralString(token);
  }
  return LiteralSInt64{atoi(token.c_str())};
}
