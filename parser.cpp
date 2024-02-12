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



using namespace std;

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




// test45 is where my error is at 


void Parser::syntax_error()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

//This function prints a syntax error message related to expressions associated with a Token and terminates the program.
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
   
    string name = recentToken.lexeme; 
    
   REG* R = parse_expr(recentToken.lexeme); // getting the copmpleted graph 
   //inputIGuess = recentToken.INPUT_TEXT;
   //cout<<inputIGuess<<"  \n"<<endl;
   globalReg  = R;
   globalName = name;

   
   regTokenPairs.push_back(make_pair(R,recentToken));

   graph = R; 
   


}

// epsilon error 
void Parser:: epsilon_error(REG* toknGraph, string name){


    // go through all the nodes and if we can go from starting state to accept state while consuming only '_' then we have epsilon. 
    // add them a set <REG_nodes*>
    set <REG_node*> nodes;
    
    nodes.insert(toknGraph->start);  
    nodes  = Match_One_Char(nodes, '_'); 


    if (nodes.find(toknGraph->accept) != nodes.end())
    {
        cout << "EPSILON IS NOOOOOOOT A TOKEN !!! "<< globalName << endl;
        // Add any other actions you want to perform if an epsilon error is found
        // For example, you might want to break out of the function or return from it
        exit(1);
    }

    // push the name onto a vector 


   
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

set<REG_node*> Parser::Match_One_Char(std::set<REG_node*> S, char c) {
    std::set<REG_node*> sPrime;

    for (REG_node* n : S){
        if(n->first_neighbor != NULL && n->first_label == c){
            sPrime.insert(n->first_neighbor);
        }

        if(n->second_neighbor != NULL && n->second_label == c){
            sPrime.insert(n->second_neighbor);
        }
    }

    if (sPrime.empty()){
        return sPrime;
    }

    bool changed = true;
    std::set<REG_node*> sDblPrime;

    while (changed) {
        changed = false;

        for (REG_node* n : sPrime) {
            sDblPrime.insert(n); 

            if (n->first_neighbor != NULL && n->first_label == '_') {
                sDblPrime.insert(n->first_neighbor);
            }

            if (n->second_neighbor != NULL && n->second_label == '_') {
                sDblPrime.insert(n->second_neighbor);
            }
        }

        if (sPrime != sDblPrime){
            changed = true;
            sPrime = sDblPrime;
            sDblPrime = std::set<REG_node*>();
        }

    }

    return sPrime;
}

// Update the function definitions in the source file
int Parser::match(REG* r, string s, int p) {
    set<REG_node*> S;
    int matchStart = -1; // Initialize matchStart to -1
    
    // Start with the set of nodes reachable from the starting node of r
    S.insert(r->start);

    S = Match_One_Char(S,'-');
    // Loop through the input string from position p
    for (int i = p; i < s.length(); ++i) {
        char c = s[i]; // Get the next character from the input string
        if (c =='"'){
            break;
        }
        if (c == ' '){
            continue;
        }
        // Find the set of nodes reachable from S by consuming the character c
        S = Match_One_Char(S, c);

        // Check if the set of reachable nodes contains the accepting node
        for (auto node : S) {
            if (node == r->accept) {
                matchStart = i; // Update matchStart if accepting node is found
                break;
            }
        }
        
        // If matchStart is not -1, a match is found, break the loop
        if (matchStart != -1) {
            break;
        }
    }

    

    // Return the starting position of the match
    return matchStart;
}

Token Parser::my_GetToken() {
    Token tokn;
    set<REG*> graphs;
    int  num = 0;
    int  longestNum = 0;
    int position = 0;
    set <Token> finalTokenSet;
    int sizeOfVector = tokens.size()-1;
    int numArry [sizeOfVector];

    for (auto pair : regTokenPairs) {
        REG* regPtr = pair.first;
        graphs.insert(regPtr);
    }

    int i =0;
    for (auto graph :graphs){
        num = match(graph, globalName,position);
        cout << num << " klsdfjmnkldsf\n";
        numArry[i] = num;
        i++;
    }

    int max = numArry[0]; // Initialize max with the first element of the array
    int maxIndex = 0; // Initialize maxIndex with the index of the first element

 // Iterate through the array starting from the second element
    for (int i = 1; i < sizeOfVector; ++i) {
    // Compare the current element with the current maximum
        if (numArry[i] > max) {
            max = numArry[i]; // Update max if the current element is greater
            maxIndex = i; // Update maxIndex with the index of the current element
        }
    }
    cout << max << " in the function\n";
    maxGlobal = max;
    tokn = tokens.at(maxIndex);



    
    return tokn; 
}

int main()
{
    
    Parser parser;

    parser.parse_all_input();
	
    if(!duplicateToken.empty()){
        cout << duplicateToken;
    }

        parser.epsilon_error(globalReg,globalName);
       

       Token tokn = parser.my_GetToken();
       string kos ;

     for (int i = 0; i < maxGlobal && i < inputText.length(); ++i) {
        kos+=inputText[i];
    }
        cout << maxGlobal <<endl;
        cout << inputText <<endl;
       cout << kos<< endl;
       cout << tokn.lexeme << ",  " << kos<<endl;
        

 return 0;
}