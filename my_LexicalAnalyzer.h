#ifndef __MY_LEXICALANALYZER_H__
#define __MY_LEXICALANALYZER_H__

#include <string>
#include <tuple>
#include <vector>
#include "parser.h"

class My_LexicalAnalyzer {
  public:
    std::tuple<Token, std::string> my_GetToken(std::vector<std::tuple<Token, struct REG*>> tokenList, std::string s, int p);
    My_LexicalAnalyzer(std::vector<std::tuple<Token, struct REG*>> tokenList, std::string s);
  private:
    Parser parser;
    int p;
    std::string s;
    std::vector<std::tuple<Token, struct REG*>> tokenList;
    void syntax_error();
    int match(struct REG *reg, std::string s, int p);
};

#endif