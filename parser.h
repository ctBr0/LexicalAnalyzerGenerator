#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>
#include <tuple>
// #include "lexer.h" from excluded source files

struct REG_node {
  struct REG_node* first_neighbor;
  char first_label;
  struct REG_node* second_neighbor;
  char second_label;
};

struct REG {
  struct REG_node* start;
  struct REG_node* accept;
};

class Parser {
  public:
    void parse_input();
    void readAndPrintAllInput();
    std::set<REG_node*> match_one_char(std::set<REG_node*> nodeList, char c);
    std::vector<std::tuple<Token, REG*>> tokenList;
    std::string inputText;
  private:
    std::vector<std::tuple<Token, Token>> semanticErrorList;
    std::vector<Token> epsilonErrorList;
    LexicalAnalyzer lexer;
    void syntax_error();
    void syntax_error_token_expr(std::string token_name);
    void epsilon_checking(Token t, REG *reg);
    Token expect(TokenType expected_type);
    Token expect_token_expr(TokenType expected_type, std::string token_name);
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    REG* parse_expr(std::string token_name);
    REG* make_one_char_REG(char x);
    REG* make_epsilon_REG();
    REG* make_or_REG(REG *reg1, REG *reg2);
    REG* make_star_REG(REG *reg);
    REG* make_dot_REG(REG *reg1, REG *reg2);
};

#endif