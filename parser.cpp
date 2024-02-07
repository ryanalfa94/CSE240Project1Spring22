/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */

// I think i did the syntax error correctly 
// and now i should be working on symantic errors 
// check for errors !



#include <iostream>
#include <cstdlib>
#include "parser.h"

using namespace std;

//unordered_map<string, int> tokenMap;

// this syntax error function needs to be 
// modified to produce the appropriate message
void Parser::syntax_error()
{
        cout << "SYNTAX ERROR\n";
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
    // i changed this from syntax_error to syntaxError
        syntax_error();
    return t;
}

void Parser::parse_expr( )
{
 // rule 
 // expr --> ChAR
 // expr --> underScore
 // expr --> (expr) | (expr)
 // expr --> (expr) . (expr)
 // expr --> (expr) *
 
    Token t = lexer.peek(1);

 if (t.token_type == CHAR) {
    expect(CHAR);
} else if (t.token_type == UNDERSCORE) {
    expect(UNDERSCORE);
} else if (t.token_type == LPAREN) {
    expect(LPAREN);
    parse_expr();
    expect(RPAREN);

    Token t_peek_1 = lexer.peek(1);

    if (t_peek_1.token_type == OR) {
        expect(OR);
        expect(LPAREN);
        parse_expr();
        expect(RPAREN);
    } else if (t_peek_1.token_type == DOT) {
        expect(DOT);
        expect(LPAREN);
        parse_expr();
        expect(RPAREN);
    } else if (t_peek_1.token_type == STAR) {
        expect(STAR);
    } else {
        string name = t_peek_1.lexeme; 
        printf("%s name of the token \n\n",name);
        syntaxError(t.lexeme);
    }
} else {
    syntax_error();
    
}




}

void Parser::parse_token()
{
   // rule 
   // token ---> ID expr

    Token mostRecentToken = expect(ID);
    //tokenMap[mostRecentToken.lexeme] = 1

    // in the video he wrote parse_expr(mostRecentToken.lexem);  double check if its lexem or lexer.   
    parse_expr(); 
    // after the line above, we should have no syntactical errors.

    // i think i should add the mostrecent token to a hashmap or something so i can keep track of the token names
    // so i can see if i have any matchingh token names so i can print an error aka we have a symantic error.   




}

void Parser::parse_token_list()
{
    //rule 
    //token-list is either 
    // token or token COMMA token-list
    parse_token();
    Token t = lexer.peek(1);
    if (t.token_type == COMMA){
        expect(COMMA);
        parse_token_list();
    }else if(t.token_type == HASH){
        return;
    }else {
        syntax_error();
    }

    
}

void Parser::parse_tokens_section()
{
    // rule:
    // token-section---> token-list hash
    parse_token_list();
    expect(HASH);
}

void Parser::parse_input()
{
    //the rule is :
    // input---> token-section(input-text)
    parse_tokens_section();
    expect(INPUT_TEXT);
    expect(END_OF_FILE);
}


// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
// This function is not needed for your solution and it is only provided to
// illustrate the basic functionality of getToken() and the Token type.

void Parser::readAndPrintAllInput()
{
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE) 
    {
        t.Print();         	// pringt token
        t = lexer.GetToken();	// and get another one
    }
        
    // note that you should use END_OF_FILE and not EOF
}

void Parser::syntaxError( std::string& name){
    //printf("%s is my lexeme \n\n\n",name);
    printf("%s HAS A SYNTAX ERROR IN ITS EXPRESSION\n",name);
    exit(1);
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

    parser.readAndPrintAllInput();
    parser.parse_input();
    // after this point it means no syntax error
    // now we need to check if ther e is as symantic error
    // we should check if the  expression has an epesilon aka ?
    // if no errors at all 
    // now we do lexical analysis!
	
}

// HELPER FUNCTIONS ARE BELLOW :


// this function return the proper syntax error with the token name.

