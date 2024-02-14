
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
#include <set> 
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <sstream>



using namespace std;

// Gloable variables that will be used later.
vector<Token> tokens;
string duplicateToken; 
REG* graph;
vector <pair <REG*, Token>> regTokenPairs;
string inputText;
//int p;




// this function deals outside errors.
void Parser::syntax_error()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

//This function deals with errors in the token.
void Parser::syntax_error_expr(string toknName)
{
    cout << toknName << " HAS A SYNTAX ERROR IN ITS EXPRESSION \n";
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

//This function gets a token from the lexer.
Token Parser::expect_expr(TokenType expected_type, string toknName)
{
     Token t = lexer.GetToken();
     if (t.token_type != expected_type)
         syntax_error_expr(toknName);
        return t;
}
// parse all the input till the end of file.
void Parser::parse_all_input()
{ 
    parse_input();
    expect(END_OF_FILE); 
}

void Parser::parse_input()
{ //input -> token section INPUT_TXT
    parse_tokens_section();
    Token tt = expect(INPUT_TEXT);

    // getting the input string and store it in inputtext.
    inputText = tt.lexeme;
    
    
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
// Parses a single token,and checking for duplicate.
void Parser::parse_token()
{ // token -> ID expr
   int flag = 0;
   string firstID = "";
   Token recentToken = expect(ID);
   // pushing the tokens to a vector 
   tokens.push_back(recentToken);

    // looping through the tokens vector
    for (auto& token : tokens){
    if (token.lexeme == recentToken.lexeme)
    {
        flag += 1;
        if (flag == 1)
        {
            firstID = to_string(token.line_no);
            continue;
        }
        // Construct error message using stringstream
        stringstream errorMsg;
        errorMsg << "Line " << recentToken.line_no << ": " << token.lexeme
                 << " already declared on line " << firstID << "\n";
        // Append error message to duplicateToken so it can be printed.
        duplicateToken += errorMsg.str();
        break;
    }
}
    
   REG* R = parse_expr(recentToken.lexeme); // getting the copmpleted graph 

   // pushing the graphs and the token to a pair vector 
   regTokenPairs.push_back(make_pair(R,recentToken)); 
}

// check for epsilon error 
void Parser:: epsilon_error(vector <pair <REG*, Token>> regTokenPairs){

    // go through all the nodes and if we can go from starting state to accept state while consuming only '_' then we have epsilon. 
    // add them a set <REG_nodes*>
    set <REG_node*> nodes;
    vector <Token> tokennames;

    // this loop deals with each ggraph and token 
    // if epsilon detected push the token to a vector so i can print them later.
   for (pair<REG*, Token> pair : regTokenPairs) {
    REG* reg = pair.first; // Accessing the first element of the pair
    Token token = pair.second; // Accessing the second element of the pair

    // add the starting node of the graphs so match one char 
    // determine if there is an error or no.
    nodes.insert(reg->start);
    nodes = Match_One_Char(nodes, '_');
    if (nodes.find(reg->accept) != nodes.end()) {
        // adding all the tokens that has an error to a vector so
        // it can be printed after all graphs are checked.
        tokennames.push_back(token);
    }
}
    // if the vector is empty that means no errrors encountered 
    // other wise print the error msg and the tokoen names. 
    if (!tokennames.empty()){
        cout << "EPSILON IS NOOOOOOOT A TOKEN !!! ";
        for (auto token : tokennames){
            cout << token.lexeme << " " ;
        }
        cout <<endl; 
        exit(1);
    }

    }




// Parses expressions associated with an identifier
REG* Parser::parse_expr(string toknName)
{ // expr -> CHAR
  // expr -> UNDERSCORE
  // expr -> (expr) | (expr)
  // expr -> (expr) DOT (expr)
  // expr -> (expr)*


    // following the pdf instructions to consume tokens/ setting up the graphs correctly
    Token t = lexer.peek(1);
    if (t.token_type == CHAR){
        // dealing with Char
        expect_expr(CHAR, toknName);
        REG_node* n1 = new REG_node;
        REG_node* n2 = new REG_node;
        REG* r = new REG;
        r->start = n1;
        r->accept = n2;
        n1->first_neighbor = n2;
        n1->first_label = t.lexeme[0];
        return r;

    } else if(t.token_type == UNDERSCORE){
        // dealing with underscore
        expect_expr(UNDERSCORE, toknName);
        REG_node* n1 = new REG_node;
        REG_node* n2 = new REG_node;
        REG* r = new REG;
        r->start = n1;
        r->accept = n2;
        n1->first_neighbor = n2;
        n1->first_label = '_';
        return r;

    } else if(t.token_type == LPAREN){
        // dealing with leftpren
        expect_expr(LPAREN, toknName);
        REG* r1 = parse_expr(toknName);
        expect_expr(RPAREN, toknName);
        // we peek at the next token so
        // that will decide if we are going to 
        // or , dot or a star.
        t = lexer.peek(1);
        if (t.token_type == OR){
            expect_expr(OR, toknName);
            expect_expr(LPAREN, toknName);
            REG* r2 = parse_expr(toknName);
            expect_expr(RPAREN, toknName);
            REG* R = uni(r1, r2);
            // we delete the tokens we made since c++ has no garbage selection (memory leak).
            delete r1;
            delete r2;
            return R;

        } else if(t.token_type == DOT){
            // dealing with dot
            expect_expr(DOT, toknName);
            expect_expr(LPAREN, toknName);
            REG* r2 = parse_expr(toknName);
            expect_expr(RPAREN, toknName);
            REG* R = concat(r1, r2);
            // we delete the tokens we made since c++ has no garbage selection (memory leak).
            delete r1;
            delete r2;
            return R;

        } else if(t.token_type == STAR){
            // dealing with Char
            expect_expr(STAR, toknName);
            REG* R = star(r1);
            // we delete the tokens we made since c++ has no garbage selection (memory leak).
            delete r1;
            return R;

        } else{
            // anything else
            syntax_error_expr(toknName);
        }
    } else{
        syntax_error_expr(toknName);
    }

}

    // Function to perform union operation on two regular expressions
    // creating the transitions needed for union 
    //for some reason i can not call it union.
REG* Parser::uni(REG* reg1, REG* reg2){
    REG_node* node1 = new REG_node;
    REG_node* node2 = new REG_node;
    REG* resultREG = new REG;

    resultREG->start = node1;
    resultREG->accept = node2;

    // Creating transitions from the new starting state to the starting states of reg1 and reg2
    // Transition 1: to the starting state of reg1 with label '_'
    node1->first_neighbor = reg1->start;
    node1->first_label = '_';

    // Creating transitions from the accept states of reg1 and reg2 to the new accept state
    // Transition from reg1's accept state to the new accept state with label '_'
    node1->second_neighbor = reg2->start;
    node1->second_label = '_';

    // Creating the final state
    reg1->accept->first_neighbor = resultREG->accept;
    reg1->accept->first_label = '_';

    reg2->accept->second_neighbor = node2;
    reg2->accept->second_label = '_';

    return resultREG;
}

    // Function to concatenate two regular expressions
REG* Parser::concat(REG* reg1, REG* reg2){
    REG* resultREG = new REG;

    // Linking the accept state of reg1 to the start state of reg2
    // Transition from the accept state of reg1 to the start state of reg2 with label '_'
    reg1->accept->first_neighbor = reg2->start;
    reg1->accept->first_label = '_';
    
    // Preserving the initial states of reg1 and the accept state of reg2 for the new REG
    // Assigning the start state of reg1 to the start state of the resulting regular expression
    resultREG->start = reg1->start;
    resultREG->accept = reg2->accept;

    return resultREG;
}

// Function to create the Kleene star operation on a regular expression
REG* Parser::star(REG* reg1){
    REG_node* startNode = new REG_node;
    REG_node* acceptNode = new REG_node;
    REG* resultREG = new REG;
    
    // Creating new start and accept states
    resultREG->start = startNode;
    resultREG->accept = acceptNode;

    // Linking the new start state with the previous start state 
    // and the new accept state
    resultREG->start->first_neighbor = reg1->start;
    resultREG->start->first_label = '_';

    resultREG->start->second_neighbor = resultREG->accept;
    resultREG->start->second_label = '_';

    // Connecting the previous accept state of reg1
    // to the previous start state and the new accept state
    reg1->accept->first_neighbor = reg1->start;
    reg1->accept->first_label = '_';

    // Transition from the accept state of reg1 to the new accept state with label '_'

    reg1->accept->second_neighbor = resultREG->accept;
    reg1->accept->second_label = '_';

    return resultREG;
}

// This function takes a set of REG_nodes 'S' and a character 'c' as input and returns a new set
// containing REG_nodes that can be reached by transitioning from the nodes in 'S' with the input character 'c'.
set<REG_node*> Parser::Match_One_Char(std::set<REG_node*> S, char c) {
    // Create an empty set to store the next set of reachable nodes
    std::set<REG_node*> sPrime;

    // Iterate over each node in the input set 'S'
    for (REG_node* n : S) {
        // Check if the node has a transition labeled with the input character 'c' and add the target node to 'sPrime'
        if (n->first_neighbor != NULL && n->first_label == c) {
            sPrime.insert(n->first_neighbor);
        }

        // Check if the node has a second transition labeled with the input character 'c' and add the target node to 'sPrime'
        if (n->second_neighbor != NULL && n->second_label == c) {
            sPrime.insert(n->second_neighbor);
        }
    }

    // If 'sPrime' is empty (no transitions found), return an empty set
    if (sPrime.empty()) {
        return sPrime;
    }

    // Initialize a boolean variable 'changed' to track changes in the set 'sPrime'
    bool changed = true;
    // Create a new set 'sDblPrime' to store the next set of reachable nodes after considering epsilon transitions
    std::set<REG_node*> sDblPrime;

    // Iterate until no more changes occur in 'sPrime'
    while (changed) {
        // Reset the 'changed' flag to false at the beginning of each iteration
        changed = false;

        // Iterate over each node in the current set 'sPrime'
        for (REG_node* n : sPrime) {
            // Add the current node 'n' to the set 'sDblPrime'
            sDblPrime.insert(n); 

            // Check if the current node 'n' has an epsilon transition labeled with '_', and add the target node to 'sDblPrime'
            if (n->first_neighbor != NULL && n->first_label == '_') {
                sDblPrime.insert(n->first_neighbor);
            }

            // Check if the current node 'n' has a second epsilon transition labeled with '_', and add the target node to 'sDblPrime'
            if (n->second_neighbor != NULL && n->second_label == '_') {
                sDblPrime.insert(n->second_neighbor);
            }
        }

        // If 'sPrime' is different from 'sDblPrime' (i.e., new nodes added), update 'sPrime' and set 'changed' to true
        if (sPrime != sDblPrime) {
            changed = true;
            sPrime = sDblPrime;
            sDblPrime = std::set<REG_node*>(); // Clear 'sDblPrime' for the next iteration
        }
    }

    // Return the final set of reachable nodes after considering epsilon transitions
    return sPrime;
}
int Parser::match(REG* R, string s, int p){

    set<REG_node*> nodes;
    // insert start nodes into set
    int pp = 0;
    nodes.insert(R->start);
    // call match one char to get all the nodes from start by consuming _
    // otherwise we wont get the correct set when we match on a letter.
    nodes = Match_One_Char(nodes, '_'); 
    nodes.insert(R->start); // insert start nodes.
    for(int i = p;  i < s.length(); i++){ // iterate through string
        nodes = Match_One_Char(nodes, s[i]); // call match one char to see if there is a character \

        if(nodes.empty()){ // if your set is empty we break.
            break;
        }
        // if we find the accept state we
        if(nodes.find(R->accept) != nodes.end())
            pp = i; // increment. 
        }

   return pp;
}

void Parser::my_GetToken(vector <pair <REG*, Token>> regTokenPair){

    Token token;
    int p = 0;
    int tmp;
    int longest = -1;
    // Looping through the string input.
    while (p < inputText.length()) {
        // skipping white spaces and ""
        if (isspace(inputText[p]) || inputText[p] == '\"') {
            p++;
            continue;
        }

        bool tokenFound = false;
        //looping through the vector.
        for (auto& pair : regTokenPair) {
            // finsing the longest match.
            tmp = match(pair.first, inputText, p);
            //check if the current match is larger than the previous match.
            if (tmp > longest) {
                // update the longest match 
                longest = tmp;
                token = pair.second;
                // this string is used to double check if the if the outputed string is empty.
                string lexeme = "";

                // Extract the lexeme corresponding to the longest match
                for (int j = p; j <= longest; j++) {
                    //skipping spaces.
                    if (!isspace(inputText[j])) {
                    lexeme += inputText[j];
                    }
                }

                //if lexeme is not an empty string that means we have a match
                // print the token name as well as that longest matched string.  
                if (!lexeme.empty()) {
                    cout << token.lexeme << ", \"" << lexeme << "\"" << endl;
                 }
        
                p = longest + 1; // Move to the next character
                tokenFound = true;  // a flag if we dont find a token.
                break; // Exit the loop after finding the first matching token
            }
        }
        // if a token not found we error out.
        if (!tokenFound) {
            cout << "ERROR" << endl;
            exit(1);
        }
    }
}




int main()
{
    
    Parser parser;

    parser.parse_all_input();
	
    if(!duplicateToken.empty()){
        cout << duplicateToken;
    }else{

        parser.epsilon_error(regTokenPairs);
        parser.my_GetToken(regTokenPairs);
        
    }

    

 return 0;
}