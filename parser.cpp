
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



using namespace std;

// some of these variables are ebugging purposes only.
set <REG*> epssilonset; 
string inputIGuess;
REG* globalReg;
string globalName;
vector<Token> tokens;
string duplicateToken; 
set <REG*> allNodes; 
REG* graph;
vector <string> vectorOfNames;
bool globalFlag = true;
vector <pair <REG*, Token>> regTokenPairs;

string inputText;
int p;
int maxGlobal = 0;





void Parser::syntax_error()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

//This function prints a syntax error message related to expressions.
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

//This function retrieves a token from the lexer and checks if it matches the expected token type.
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
// parse the token section that passed in the vector.
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
                duplicateToken += "Line " + to_string(recentToken.line_no) +": "+ tokens[i].lexeme + " already declared on line " + firstID + "\n";
                break;
            }
        }
   
    // store n name and the graph in a gloable variable to have access to them in case i need them.
    string name = recentToken.lexeme;  
   REG* R = parse_expr(recentToken.lexeme); // getting the copmpleted graph 

   globalReg  = R;
   globalName = name;

   // pushing the graphs and the token to a pair vector 
   regTokenPairs.push_back(make_pair(R,recentToken));
    epssilonset.insert(R);
   graph = R; 

   


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

    nodes.insert(reg->start);
    nodes = Match_One_Char(nodes, '_');
    if (nodes.find(reg->accept) != nodes.end()) {
        tokennames.push_back(token);
    }
}

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

    Token t = lexer.peek(1);
    if (t.token_type == CHAR){
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
        expect_expr(LPAREN, toknName);
        REG* r1 = parse_expr(toknName);
        expect_expr(RPAREN, toknName);
        t = lexer.peek(1);
        if (t.token_type == OR){
            expect_expr(OR, toknName);
            expect_expr(LPAREN, toknName);
            REG* r2 = parse_expr(toknName);
            expect_expr(RPAREN, toknName);
            REG* R = uni(r1, r2); 
            delete r1;
            delete r2;
            return R;
        } else if(t.token_type == DOT){
            expect_expr(DOT, toknName);
            expect_expr(LPAREN, toknName);
            REG* r2 = parse_expr(toknName);
            expect_expr(RPAREN, toknName);
            REG* R = concat(r1, r2);
            delete r1;
            delete r2;
            return R;
        } else if(t.token_type == STAR){
            expect_expr(STAR, toknName);
            REG* R = star(r1);
            delete r1;
            return R;
        } else{
            syntax_error_expr(toknName);
        }
    } else{
        syntax_error_expr(toknName);
    }

}

    // connecting 2 nodes as needed 
REG* Parser::uni(REG* reg1, REG* reg2){
    REG_node* node1 = new REG_node;
    REG_node* node2 = new REG_node;
    REG* resultREG = new REG;
    resultREG->start = node1;
    resultREG->accept = node2;

    // Creating the starting state
    node1->first_neighbor = reg1->start;
    node1->first_label = '_';

    node1->second_neighbor = reg2->start;
    node1->second_label = '_';

    // Creating the final state
    reg1->accept->first_neighbor = resultREG->accept;
    reg1->accept->first_label = '_';

    reg2->accept->second_neighbor = node2;
    reg2->accept->second_label = '_';

    return resultREG;
}

    // connecting 2 nodes as needed 
REG* Parser::concat(REG* reg1, REG* reg2){
    REG* resultREG = new REG;

    // Linking the previous accept state of reg1
    // to the initial state of reg2
    reg1->accept->first_neighbor = reg2->start;
    reg1->accept->first_label = '_';
    
    // Preserving the initial states of reg1
    // and the accept state of reg2
    // for the new REG
    resultREG->start = reg1->start;
    resultREG->accept = reg2->accept;

    return resultREG;
}


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
    nodes.insert(R->start);
    nodes = Match_One_Char(nodes, '_'); // call epsilon check to see if there is epsilon error

    nodes.insert(R->start); // insert start nodes again to update set

    int pPrime = 0;

    for(int i = p;  i < s.length(); i++){ // iterate through string

          if(isspace(s[p])){
            break;
        }

        // handle the case where I see "
        if(s[p] == '\"'){
            break;
        } 

        nodes = Match_One_Char(nodes, s[i]); // call match one char to see if there is a character 

        if(nodes.empty()){ // if your set is empty then you are going to break the loop
            break;
        }

        if(nodes.find(R->accept) != nodes.end())
            pPrime = i; // increment pPrime
        }

   return pPrime;
}

void Parser::my_GetToken(std::vector <pair <REG*, Token>> regTokenPair){

    Token token;
    int p = 0;
    int tmp;
    int longest = -1;

    while (p < inputText.length()) {
        if (isspace(inputText[p]) || inputText[p] == '\"') {
            p++;
            continue;
        }

        bool tokenFound = false;

        for (int i = 0; i < regTokenPair.size(); i++) {
            tmp = match(regTokenPair[i].first, inputText, p);
            if (tmp > longest) {
                longest = tmp;
                token = regTokenPair[i].second;
                cout << token.lexeme << ", \"";

                for (int j = p; j <= longest; j++) {
                    if (!isspace(inputText[j])) {
                        cout << inputText[j];
                    }
                }
                cout << "\"" << endl;
                p = longest + 1; // Move to the next character after the consumed token
                tokenFound = true;
                break; // Exit the loop after finding the first matching token
            }
        }
        
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