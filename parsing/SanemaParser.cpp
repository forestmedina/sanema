//
// Created by fores on 23/3/2022.
//

#include "SanemaParser.hpp"
#include <set>
#include <algorithm>
#include "expressions.h"
#include <iostream>
#include <istream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

sanema::BlockOfCode sanema::SanemaParser::parse(const std::vector<sanema::Token> &tokens) {
  auto current = tokens.begin();
  auto end = tokens.end();
  return parse_block(current, end);
}

sanema::BlockOfCode sanema::SanemaParser::parse_block(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
  BlockOfCode block;
  while (current != end) {
    if (current->token == code_block_end || current->token == if_else_word || current->token == if_ending_word) {
      break;
    }
    auto instruction = parse_instruction(current, end);
    if (instruction.has_value()) {
      block.instructions.emplace_back(instruction.value());
    }
  }
  return block;
}

std::optional<sanema::Instruction> sanema::SanemaParser::parse_instruction(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
  if (current == end) return {};

  if (current->token == variable_declaring_word) {
    return parse_variable_declaration(current, end);
  } else if (current->token == function_declaring_word) {
    return parse_function_definition(current, end);
  } else if (current->token == struct_declaring_word) {
    return parse_struct_definition(current, end);
  } else if (current->token == if_word) {
    return parse_if_statement(current, end);
  } else if (current->token == for_loop_word) {
    return parse_for_statement(current, end);
  } else if (current->token == return_word) {
    return parse_return_statement(current, end);
  } else if (current->token == code_block_begin) {
    current++; // consume 'begin'
    auto block = parse_block(current, end);
    if (current != end && current->token == code_block_end) {
      current++; // consume 'end'
    }
    return block;
  } else if (!is_delimiter(current->token)) {
    std::string identifier = current->token;
    current++;
    if (current != end && current->token == "(") {
      return parse_function_call(identifier, current, end);
    } else {
       // Backtrack to let parse_function_call handle the identifier
       current--;
       return parse_function_call(current->token, current, end);
    }
  }

  current++; // Skip unknown or delimiter
  return {};
}

sanema::DeclareVariable sanema::SanemaParser::parse_variable_declaration(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
  DeclareVariable declare_variable;
  current++; // consume 'var'

  if (current != end) {
    declare_variable.identifier = current->token;
    current++;
  }

  if (current != end) {
    declare_variable.type_identifier = parse_type(current->token).value();
    current++;
  }

  if (current != end && current->token == ";") {
    current++;
  }

  return declare_variable;
}

sanema::DefineFunction sanema::SanemaParser::parse_function_definition(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
  DefineFunction define_function;
  current++; // consume 'function'

  if (current != end) {
    define_function.identifier = current->token;
    current++;
  }

  if (current != end) {
    define_function.type = parse_type(current->token).value();
    current++;
  }

  while (current != end && current->token != std::string("") + code_block_begin) {
      // Parse parameters
      FunctionParameterIncomplete param;
      param.modifier = parse_modifier(current->token);
      current++;

      if (current != end) {
          param.identifier = current->token;
          current++;
      }

      if (current != end) {
          param.type = parse_type(current->token);
          current++;
      }
      define_function.parameters.push_back(param);
  }

  if (current != end && current->token == std::string("") + code_block_begin) {
    current++; // consume 'begin'
    define_function.body = parse_block(current, end);
    if (current != end && current->token == code_block_end) {
      current++; // consume 'end'
    }
  }

  return define_function;
}

sanema::DefineStruct sanema::SanemaParser::parse_struct_definition(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
  DefineStruct define_struct;
  current++; // consume 'struct'

  if (current != end) {
    define_struct.type_id = TypeIdentifier(current->token);
    current++;
  }

  while (current != end && current->token != ";") {
      IncompleteField field;
      if (current != end) {
          field.identifier = current->token;
          current++;
      }
      if (current != end) {
          field.type = parse_type(current->token);
          current++;
      }
      define_struct.fields.push_back(field);

      if (current != end && current->token == ",") {
          current++;
      }
  }

  if (current != end && current->token == ";") {
      current++;
  }

  return define_struct;
}

sanema::IfStatement sanema::SanemaParser::parse_if_statement(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
  IfStatement if_statement;
  current++; // consume 'if'

  if_statement.expression = parse_expression(current, end);

  if (current != end && current->token == code_block_begin) {
      current++; // consume 'begin'
      if_statement.true_path = parse_block(current, end);

      if (current != end) {
          if (current->token == if_else_word) {
              current++; // consume 'else'
              // Check if else is followed by begin or implicit block
              if (current != end && current->token == code_block_begin) {
                   current++; // consume 'begin'
                   if_statement.false_path = parse_block(current, end);
                   if (current != end && current->token == if_ending_word) {
                       current++; // consume 'end'
                   }
              } else {
                  if_statement.false_path = parse_block(current, end);
                   if (current != end && current->token == if_ending_word) {
                       current++; // consume 'end'
                   }
              }
          } else if (current->token == if_ending_word) {
              current++; // consume 'end'
          } else if (current->token == code_block_end) {
              current++; // consume 'end'
          }
      }
  }

  return if_statement;
}

sanema::ReturnStatement sanema::SanemaParser::parse_return_statement(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
    ReturnStatement return_statement;
    current++; // consume 'return'

    if (current != end && current->token == ";") {
        current++;
        return return_statement;
    }

    return_statement.expression = parse_expression(current, end);

    if (current != end && current->token == ";") {
        current++;
    }
    return return_statement;
}

sanema::ForStatement sanema::SanemaParser::parse_for_statement(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
    ForStatement for_statement;
    current++; // consume 'for'
    return for_statement;
}

sanema::FunctionCall sanema::SanemaParser::parse_function_call(std::string identifier, std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
    FunctionCall function_call;
    function_call.identifier = identifier;

    if (current != end && current->token == identifier) {
        current++;
    }

    if (current != end && current->token == "(") {
        current++; // consume '('
    }

    while (current != end) {
        if (current->token == ")" || current->token == ";") {
            current++;
            break;
        }

        if (current->token == ",") {
            current++;
            continue;
        }

        function_call.arguments.emplace_back(FunctionArgument{parse_expression(current, end)});
    }

    return function_call;
}

sanema::Expression sanema::SanemaParser::parse_expression(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end) {
    if (current == end) return VariableEvaluation{""};

    if (is_literal(current->token)) {
        auto literal = get_literal_from_string(current->token);
        current++;
        return literal;
    }

    std::string identifier = current->token;
    current++;

    if (current != end && current->token == "(") {
        current--;
        return parse_function_call(identifier, current, end);
    }

    return VariableEvaluation{identifier};
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
