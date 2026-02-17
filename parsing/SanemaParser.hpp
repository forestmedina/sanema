//
// Created by fores on 23/3/2022.
//

#ifndef NATURE_SANEMA_HPP
#define NATURE_SANEMA_HPP

#include "expressions.h"
#include <vector>
#include <variant>
#include <set>
#include <optional>

namespace sanema {

  struct Token {
    Token(std::string &&token, long lineNumber, int columnNumber);


    std::string token;
    long line_number{0};
    long column_number{0};
  };

  struct ParsingError {

  };

  class SanemaParser {
  public:
    SanemaParser();


    BlockOfCode parse(std::vector<Token> const &tokens);

    std::vector<Token> tokenize(std::istream &text);

  private:
    bool is_delimiter(std::string const &string);

    bool is_literal(std::string const &string);

    Literal get_literal_from_string(std::string token);

    // Recursive parsing helpers
    BlockOfCode parse_block(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    std::optional<Instruction> parse_instruction(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    Expression parse_expression(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    DefineFunction parse_function_definition(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    DeclareVariable parse_variable_declaration(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    IfStatement parse_if_statement(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    FunctionCall parse_function_call(std::string identifier, std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    DefineStruct parse_struct_definition(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    ReturnStatement parse_return_statement(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);
    ForStatement parse_for_statement(std::vector<Token>::const_iterator &current, const std::vector<Token>::const_iterator &end);

    std::string code_block_begin = "begin";
    std::string code_block_end = "end";
    std::string variable_declaring_word = "var";
    std::string function_declaring_word = "function";
    std::string struct_declaring_word = "struct";
    std::string if_word = "if";
    std::string if_else_word = "else";
    std::string if_ending_word = "end";
    std::string return_word = "return";
    std::string for_loop_word= "repeat";
    std::set<std::string> reserved_words{variable_declaring_word, function_declaring_word, struct_declaring_word, for_loop_word};
    std::set<char> separators = {' ', '\t'};
    std::set<char> function_delimiters = {'(', ')'};
    std::set<char> terminators = {';'};
    std::set<char> field_separator = {','};
    std::set<char> string_delimiters = {'"'};
//    std::set<char> code_block_delimiters = {code_block_begin, code_block_end};
    std::set<char> line_ending_character = {'\n'};
    std::set<char> ignored_characters = {'\r'};
    std::set<char> delimiters;
  };
}


#endif //NATURE_SANEMA_HPP
