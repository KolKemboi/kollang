#include "headers/AST.hpp"
#include "headers/AsmGen.hpp"
#include "headers/IR.hpp"
#include "headers/Lexer.hpp"
#include "headers/Parser.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
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

  std::ifstream fileData;
  std::string Code;

  fileData.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    fileData.open(argv[1]);
    std::stringstream codeStream;
    codeStream << fileData.rdbuf();
    fileData.close();

    Code = codeStream.str();
  } catch (std::ifstream::failure e) {
    printf("%s\n", e.what());
    std::exit(1);
  }

  std::string_view SourceCode = Code;

  printf("Source code\n%s", std::string(SourceCode).c_str());
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
    printf("%s => %s(%s)\n", instr.s_Type.c_str(), instr.s_Operand.c_str(),
           instr.s_Operation.c_str());
  }
  printf("============ASM GENERATION==================\n");
  AsmGen asmGenerator = AsmGen(instructions);
  asmGenerator.generateASM();
}
