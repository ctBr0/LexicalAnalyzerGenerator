#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <tuple>
#include "my_LexicalAnalyzer.h"

using namespace std;

void My_LexicalAnalyzer::syntax_error()
{
    cout << "ERROR";
    exit(1);
}

tuple<Token, string> My_LexicalAnalyzer::my_GetToken(vector<tuple<Token, REG*>> tokenList, string s, int p)
{   
    int prefixEndIndex;
    tuple<Token, string> longest;
    int currentIndex = p - 1; //no matches
    for (tuple<Token, REG*> token : tokenList)
    {
        prefixEndIndex = match(get<1>(token), s, p);
        if (prefixEndIndex > currentIndex)
        {
            longest = make_tuple(get<0>(token), s.substr(p, prefixEndIndex - p + 1)); // combine the longest substring and its respective token into a tuple
            currentIndex = prefixEndIndex;
        }
    }
    if (currentIndex == p - 1) //no matches
    {
        syntax_error();
    }
    return longest;
}

int My_LexicalAnalyzer::match(REG *reg, string s, int p)
{
    set<REG_node*> newList;
    newList.insert(reg->start);
    int prefixEndIndex = p - 1; //no matches

    set<REG_node*> reachableNodeList = parser.match_one_char(newList, '_');
    reachableNodeList.insert(reg->start);
    
    while ((!reachableNodeList.empty()) && (p < s.length() - 1))
    {
        reachableNodeList = parser.match_one_char(reachableNodeList, s[p]);
        if (reachableNodeList.find(reg->accept) != reachableNodeList.end()) // the accept node is reachable
        {
            prefixEndIndex = p;
        }
        p++;
    }
    return prefixEndIndex;
}

My_LexicalAnalyzer::My_LexicalAnalyzer(vector<tuple<Token, REG*>> tokenList, string s)
{
    this->tokenList = tokenList;
    this->s = s;
    this->p = 1; //skip over first " in parser.inputText
    
    while (s[p] == ' ') // skip over spaces
    {
        p++;
    }

    while (p < s.length() - 1) //omitting last " in parser.inputText
    {
        tuple<Token, string> token = my_GetToken(tokenList, s, p);
        cout << get<0>(token).lexeme + " , \"" + get<1>(token) + "\"" << endl; 
        p = p + get<1>(token).length();
        while (s[p] == ' ')
        {
            p++;
        }   
    }
}

int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object. 
    // You can access the lexer object in the parser functions as shown in 
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will )
    Parser parser;

    parser.parse_input();

    My_LexicalAnalyzer(parser.tokenList, parser.inputText);
}
