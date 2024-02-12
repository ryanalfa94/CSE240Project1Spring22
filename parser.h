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
#include "struct.h"
#include <set>
#include <unordered_set>


using namespace std;

class Parser {
  public:
    void parse_all_input();
    void parse_input();
    void readAndPrintAllInput();
    int match(REG* r, string s, int p);
    void epsilon_error(REG* toknGraph, string name);
    Token my_GetToken();
    
  private:
 LexicalAnalyzer lexer;
    void syntax_error();
    void syntax_error_expr(std::string toknName);
    Token expect(TokenType expected_type);
    Token expect_expr(TokenType expected_type, std::string toknName);
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    REG* parse_expr(std::string toknName);
    REG* uni(REG* reg1, REG* reg2);
    REG* concat(REG* reg1, REG* reg2);
    REG* star(REG* reg1);
    std::set<REG_node*> Match_One_Char(std::set<REG_node*> S, char c);
    


};

#endif

