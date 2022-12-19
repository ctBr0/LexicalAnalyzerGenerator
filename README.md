# LexicalAnalyzerGenerator

Selected source files of university course project. <br />
<br />
Input: <br />
List of tokens separated by commas and terminated with the hash (#) symbol <br />
Each token consists of a name and a description (using regular expressions) <br />
Input string <br />
<br />
Output: <br />
Sequence of tokens and lexemes OR <br />
Syntax errors OR <br />
Semantic errors OR <br />
Errors when regex of token generates the empty string <br />
<br />
Implementation: <br />
Tokens are parsed and their regexes are transformed into graphs. <br />
Substrings of the input string are then matched against the existing regex graphs.
