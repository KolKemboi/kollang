#include "headers/AST.hpp"
#include "headers/Lexer.hpp"
#include "headers/Parser.hpp"
#include <cstdio>
#include <string>
#include <variant>
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

template <typename... Ts> struct U : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> U(Ts...) -> U<Ts...>;

int main(int argc, char *argv[]) {
  std::string_view SourceCode =
      "int main () { int int_name = 10; bul bool_name = "
      "true; flt float_name = 1.009; str str_name = \"string\"; ret 1;}";
  printf("Source code \n\t %s\n", std::string(SourceCode).c_str());
  Lexer lexer = Lexer(SourceCode);
  std::vector<Token> tokens = lexer.tokenize();
  PrintTokens(tokens);

  Parser parser = Parser(tokens);
  Program prog = parser.parse();

	printf("============PARSING==================\n");
  for (auto &stmt : prog.s_Main.s_Statements) {
    std::visit(
        U{
            [](VariableDeclaration &var) {
              printf("%s => ", var.s_Name.c_str());
              std::visit(U{
                             [](IntegerLiteral &iL) {
                               printf("Integer Literal, %d\n", iL.s_Value);
                             },
                             [](FloatLiteral &fL) {
                               printf("Float Literal, %f\n", fL.s_Value);
                             },
                             [](StringLiteral &sL) {
                               printf("String Literal %s\n",
                                      sL.s_Value.c_str());
                             },
                             [](BooleanLiteral &bL) {
                               printf("Boolean Literal %d\n", bL.s_Value);
                             },

                         },
                         var.s_Initializer);
            },
            [](ReturnStatement &ret) {
              std::visit(U{
                             [](IntegerLiteral &iL) {
                               printf("Integer Literal, %d\n", iL.s_Value);
                             },
                             [](FloatLiteral &fL) {
                               printf("Float Literal, %f\n", fL.s_Value);
                             },
                             [](StringLiteral &sL) {
                               printf("String Literal %s\n",
                                      sL.s_Value.c_str());
                             },
                             [](BooleanLiteral &bL) {
                               printf("Boolean Literal %d\n", bL.s_Value);
                             },

                         },
                         ret.s_Expression);
            },

        },
        stmt);
  }
}
