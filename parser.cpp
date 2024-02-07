/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */

// Ryan Alfa


#include <iostream>
#include <cstdlib>
#include "parser.h"

using namespace std;

vector<Token> tokens;
string duplicateToken; 

//This function prints a syntax error message and terminates the program.
void Parser::syntax_error()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

//This function prints a syntax error message related to expressions associated with a Token and terminates the program.
void Parser::syntax_error_expr(string nameID)
{
    cout << nameID << " HAS A SYNTAX ERROR IN ITS EXPRESSION \n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi

Token Parser::expect(TokenType expected_type)
{
     Token t = lexer.GetToken();
     if (t.token_type != expected_type)
         syntax_error();
     return t;
}

//This function retrieves a token from the lexer and checks if it matches the expected token type.
Token Parser::expect_expr(TokenType expected_type, string nameID)
{
     Token t = lexer.GetToken();
     if (t.token_type != expected_type)
         syntax_error_expr(nameID);
     return t;
}
// parse all the input till the end of file.
void Parser::parse_all_input()
{ 
    parse_input();
    expect(END_OF_FILE);
}
// parse the token section that passed in the vector.
void Parser::parse_input()
{ //input -> token section INPUT_TXT
    parse_tokens_section();
    expect(INPUT_TEXT);
}
// this function parses the token list.
void Parser::parse_tokens_section()
{ //token-section -> token_list HASH
    parse_token_list();
    expect(HASH);
}
//  Parses a list of tokens, handling token lists separated by commas
void Parser::parse_token_list()
{ //token_list -> token
  //token_list -> token COMMA token_list
    parse_token();
    Token t = lexer.peek(1);
    if (t.token_type == COMMA){
        expect(COMMA);
        parse_token_list();
    } else if (t.token_type == HASH){
        return;
    } else{
        syntax_error();
    }
}
// Parses a single token, and push it to a  vector and checking for duplicate.
void Parser::parse_token()
{ // token -> ID expr
   int flag = 0;
   string firstID = "";
   Token recentToken = expect(ID);
   tokens.push_back(recentToken);
    for (int i = 0; i < tokens.size(); ++i){
            if(tokens[i].lexeme == recentToken.lexeme){
                flag = flag + 1;
                if (flag == 1){
                    firstID = to_string(tokens[i].line_no);
                    continue;
                }
                duplicateToken += "Line " + to_string(recentToken.line_no) +": "
                + tokens[i].lexeme + 
                " already declared on line " + 
                firstID + "\n";
                break;
            }
        }
   

   parse_expr(recentToken.lexeme);
}
// Parses expressions associated with an identifier
void Parser::parse_expr(string nameID)
{ // expr -> CHAR
  // expr -> UNDERSCORE
  // expr -> (expr) | (expr)
  // expr -> (expr) DOT (expr)
  // expr -> (expr)*

    Token t = lexer.peek(1);
    if (t.token_type == CHAR){
        expect_expr(CHAR, nameID);
    } else if(t.token_type == UNDERSCORE){
        expect_expr(UNDERSCORE, nameID);
    } else if(t.token_type == LPAREN){
        expect_expr(LPAREN, nameID);
        parse_expr(nameID);
        expect_expr(RPAREN, nameID);
        t = lexer.peek(1);
        if (t.token_type == OR){
            expect_expr(OR, nameID);
            expect_expr(LPAREN, nameID);
            parse_expr(nameID);
            expect_expr(RPAREN, nameID);
        } else if(t.token_type == DOT){
            expect_expr(DOT, nameID);
            expect_expr(LPAREN, nameID);
            parse_expr(nameID);
            expect_expr(RPAREN, nameID);
        } else if(t.token_type == STAR){
            expect_expr(STAR, nameID);
        } else{
            syntax_error_expr(nameID);
        }
    } else{
        syntax_error_expr(nameID);
    }

}





int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object. 
    // You can access the lexer object in the parser functions as shown in 
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will 
    // not work correctly
    Parser parser;

    parser.parse_all_input();
	
    if(!duplicateToken.empty()){
        cout << duplicateToken;
    }

}