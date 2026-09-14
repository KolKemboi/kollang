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
/*!
 * @mainpage KolLang is a c-style language,
 * The extension of KolLang is a .kol file,
 * in this project, a main.kol has been provided
 * as shown below
 *
 *  int main () {\n
 *  int int_name = 0;\n
 *  bul bool_name = true;\n
 *  str str_name = "string";\n
 *  return 1;\n
 *  }
 *
 *  the flow goes as
 *
 *  main.kol -> Lexer -> Parser -> (IRGenerator)Intermediate Representation ->
 * (AsmGen)Assembly Code Generation
 *
 */

template <typename... Ts> struct S : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> S(Ts...) -> S<Ts...>;

/**
 * @mainpage
 *
 * the main function accepts the file and produces an i386 assembly
 *
 */
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
  /*!
   * @mainpage Lexer init
   *
   */
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
