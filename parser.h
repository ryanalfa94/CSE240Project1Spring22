/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
// Ryan Alfa
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

class Parser {
  public:
    void parse_all_input();
    void parse_input();
    void readAndPrintAllInput();
  private:
 LexicalAnalyzer lexer;
    void syntax_error();
    void syntax_error_expr(std::string nameID);
    Token expect(TokenType expected_type);
    Token expect_expr(TokenType expected_type, std::string nameID);
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    void parse_expr(std::string nameID);
};

#endif

