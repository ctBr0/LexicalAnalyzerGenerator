#include <iostream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <set>
#include "parser.h"

using namespace std;

// syntax error outside token expression
void Parser::syntax_error()
{
    cout << "SNYTAX ERORR";
    exit(1);
}

// syntax error inside token expression
void Parser::syntax_error_token_expr(string token_name)
{
    cout << "SYNTAX ERROR IN EXPRESSION OF " + token_name;
    exit(1);
}

void Parser::epsilon_checking(Token t, REG *reg)
{
    set<REG_node*> startList;
    startList.insert(reg->start);

    set<REG_node*> reachableNodeList = match_one_char(startList, '_'); // find all nodes reachable from the start by just epsilons

    if (reachableNodeList.find(reg->accept) != reachableNodeList.end())
    {
        epsilonErrorList.push_back(t); // add token to vector to print later
    }
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, syntax_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
    {
        syntax_error();
    }
    if (t.token_type == INPUT_TEXT)
    {
        inputText = t.lexeme; // for lexical analysis
    }
    return t;
}

// expect function inside token expression and takes the token name as an argument
Token Parser::expect_token_expr(TokenType expected_type, string token_name)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error_token_expr(token_name);
    return t;
}

// expect, parse and create REGs
REG* Parser::parse_expr(string token_name)
{
    Token t = lexer.peek(1);

    if (t.token_type == CHAR)
    {
        expect_token_expr(CHAR, token_name);
        return make_one_char_REG(t.lexeme[0]);
    }
    else if (t.token_type == UNDERSCORE)
    {
        expect_token_expr(UNDERSCORE, token_name);
        return make_epsilon_REG();
    }
    else if (t.token_type == LPAREN)
    {
        expect_token_expr(LPAREN, token_name);
        REG* reg1 = parse_expr(token_name);
        expect_token_expr(RPAREN, token_name);

        t = lexer.peek(1);

        if (t.token_type == STAR)
        {
            expect_token_expr(STAR, token_name);
            return make_star_REG(reg1);
        }
        else if (t.token_type == DOT)
        {
            expect_token_expr(DOT, token_name);
            expect_token_expr(LPAREN, token_name);
            REG* reg2 = parse_expr(token_name);
            expect_token_expr(RPAREN, token_name);
            return make_dot_REG(reg1, reg2);
        }
        else if (t.token_type == OR)
        {
            expect_token_expr(OR, token_name);
            expect_token_expr(LPAREN, token_name);
            REG* reg2 = parse_expr(token_name);
            expect_token_expr(RPAREN, token_name);
            return make_or_REG(reg1, reg2);
        }
        else
        {
            syntax_error_token_expr(token_name);
        }
    }
    else
    {
        syntax_error_token_expr(token_name);
    }
}

void Parser::parse_token()
{
    Token t1 = expect(ID);

    bool existsAlready = false;
    for (tuple<Token, REG*> t2 : tokenList) 
    {
        if (t1.lexeme == get<0>(t2).lexeme)
        {
            existsAlready = true;
            semanticErrorList.push_back(make_tuple(t1, get<0>(t2))); // add both tokens to vector to print later
        }
    }

    REG* reg = parse_expr(t1.lexeme);
    epsilon_checking(t1, reg);

    if (existsAlready == false)
    {   
        tokenList.push_back(make_tuple(t1, reg)); // vector tokenList keeps track of existing tokens
    }
}

void Parser::parse_token_list()
{
    parse_token();

    Token t = lexer.peek(1);

    if (t.token_type == COMMA)
    {
        expect(COMMA);
        parse_token_list();
    }
    else if (t.token_type == HASH)
    {
        return; // HASH is not consumed here
    }
    else
    {
        syntax_error();
    }
}

void Parser::parse_tokens_section()
{
    parse_token_list();
    expect(HASH); // HASH is consumed here
}

void Parser::parse_input()
{
    parse_tokens_section();
    expect(INPUT_TEXT);
    expect(END_OF_FILE);

    // print the semantic errors
    if (!semanticErrorList.empty())
    {
        for (tuple<Token, Token> token_pair : semanticErrorList)
        {
            cout << "Line " + to_string(get<0>(token_pair).line_no) + ": " + get<0>(token_pair).lexeme + " already declared on line " + to_string(get<1>(token_pair).line_no) << endl;
        }
        exit(1);
    }

    // if there are no semantic errors, print epsilon errors
    if (!epsilonErrorList.empty())
    {
        cout << "EPSILON IS NOOOOOOOT A TOKEN !!!";

        for (Token token : epsilonErrorList)
        {
            cout << " " + token.lexeme;
        }
        exit(1);
    }
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does

void Parser::readAndPrintAllInput()
{
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE) 
    {
        t.Print();         	// print token
        t = lexer.GetToken();	// and get another one
    }
        
    // note that you should use END_OF_FILE and not EOF
}

REG* Parser::make_one_char_REG(char x)
{
    struct REG *head = new struct REG;
    struct REG_node *node1 = new struct REG_node;
    struct REG_node *node2 = new struct REG_node;

    head->start = node1;
    head->accept = node2;

    node1->first_neighbor = node2;
    node1->first_label = x;

    return head;
}

REG* Parser::make_epsilon_REG()
{
    return make_one_char_REG('_');
}

REG* Parser::make_or_REG(REG *reg1, REG *reg2)
{
    struct REG *head = new struct REG;
    struct REG_node *node1 = new struct REG_node;
    struct REG_node *node2 = new struct REG_node;

    head->start = node1;
    head->accept = node2;

    node1->first_neighbor = reg1->start;
    node1->first_label = '_';
    node1->second_neighbor = reg2->start;
    node1->second_label = '_';

    reg1->accept->first_neighbor = node2;
    reg1->accept->first_label = '_';
    reg2->accept->first_neighbor = node2;
    reg2->accept->first_label = '_';

    return head;
}

REG* Parser::make_star_REG(REG *reg)
{
    struct REG *head = new struct REG;
    struct REG_node *node1 = new struct REG_node;
    struct REG_node *node2 = new struct REG_node;

    head->start = node1;
    head->accept = node2;

    node1->first_neighbor = reg->start;
    node1->first_label = '_';
    node1->second_neighbor = node2;
    node1->second_label = '_';

    reg->accept->first_neighbor = reg->start;
    reg->accept->first_label = '_';
    reg->accept->second_neighbor = node2;
    reg->accept->second_label = '_';

    return head;
}

REG* Parser::make_dot_REG(REG *reg1, REG *reg2)
{
    struct REG *head = new struct REG;

    head->start = reg1->start;
    head->accept = reg2->accept;

    reg1->accept->first_neighbor = reg2->start;
    reg1->accept->first_label = '_';

    return head;
}

// to be used inside match() in my_LexicalAnalyzer.cc
set<REG_node*> Parser::match_one_char(set<REG_node*> nodeList, char c)
{
    set<REG_node*> reachableNodeListChar = {};

    for (REG_node* node : nodeList)
    {
        if ((node->first_label == c) && (reachableNodeListChar.find(node->first_neighbor) == reachableNodeListChar.end()))
        {
            reachableNodeListChar.insert(node->first_neighbor);
        }

        if ((node->second_label == c) && (reachableNodeListChar.find(node->second_neighbor) == reachableNodeListChar.end()))
        {
            reachableNodeListChar.insert(node->second_neighbor);
        }
    }

    if (reachableNodeListChar.empty())
    {
        return reachableNodeListChar;
    }

    bool changed = true;
    set<REG_node*> reachableNodeListEpsilon = {};

    while (changed)
    {
        changed = false;
        for (REG_node* node : reachableNodeListChar)
        {
            reachableNodeListEpsilon.insert(node);

            if ((node->first_label == '_') && (reachableNodeListEpsilon.find(node->first_neighbor) == reachableNodeListEpsilon.end()))
            {
                reachableNodeListEpsilon.insert(node->first_neighbor);
            }

            if ((node->second_label == '_') && (reachableNodeListEpsilon.find(node->second_neighbor) == reachableNodeListEpsilon.end()))
            {
                reachableNodeListEpsilon.insert(node->second_neighbor);
            }
        }

        if (reachableNodeListChar != reachableNodeListEpsilon)
        {
            changed = true;
            reachableNodeListChar = reachableNodeListEpsilon;
            reachableNodeListEpsilon.clear();
        }
    }

    return reachableNodeListChar;
}