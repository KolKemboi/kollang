#include "headers/AST.hpp"
#include "headers/IR.hpp"
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

template <typename... Ts> struct S : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> S(Ts...) -> S<Ts...>;

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
        S{
            [](VariableDeclaration &var) {
              printf("%s => ", var.s_Name.c_str());
              std::visit(S{
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
              std::visit(S{
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
  printf("============IR GENERATION==================\n");

  IRGenerator irGen;
  std::vector<IRInst> instructions = irGen.generate(prog);

  for (IRInst &instr : instructions) {
    printf("%s(%s)\n", instr.s_Operand.c_str(), instr.s_Operation.c_str());
  }
}
