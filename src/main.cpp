#include "headers/AST.hpp"
#include "headers/Lexer.hpp"
#include "headers/Parser.hpp"
#include <cstdio>
#include <string>
#include <vector>

/////LANGUAGE STRUCTURE
/*
 * int main () {
 * int int_name = 0;
 * bool bool_name = true;
 * str str_name = "string";
 * return 1;
 * }
 *
 */

int main(int argc, char *argv[]) {
  std::string_view SourceCode =
      "int main () { int int_name = 0; bul bool_name = "
      "false; flt float_name = 1.009; str str_name = \"string\"; ret 1;}";
  printf("Source code \n\t %s\n", std::string(SourceCode).c_str());
  Lexer lexer = Lexer(SourceCode);
  std::vector<Token> tokens = lexer.tokenize();
  PrintTokens(tokens);

  Parser parser = Parser(tokens);
  Program prog = parser.parse();
}
