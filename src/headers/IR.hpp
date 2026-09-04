#pragma once

#include "AST.hpp"
#include <cstdio>
#include <string>
#include <variant>
#include <vector>

struct IRInst {
  std::string s_Operation;
  std::string s_Operand;

  // IRInst(std::string op, std::string operand)
  //     : s_Operation(op), s_Operand(operand) {};
};

template <typename... Ts> struct U : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> U(Ts...) -> U<Ts...>;

class IRGenerator {
public:
  std::vector<IRInst> generate(Program &program) {
    std::vector<IRInst> instruction;
    std::vector<Statement> statements = program.s_Main.s_Statements;

    // std::visit Statements{VarDec, RetStmt};
    // std::v
    for (Statement &stmt : statements) {
      IRInst inst;

      std::visit(
          U{
              [&](VariableDeclaration &varDec) {
                std::visit(U{
                               [&](IntegerLiteral &iL) {
                                 inst.s_Operand = varDec.s_Name;
                                 inst.s_Operation = std::to_string(iL.s_Value);
                               },
                               [&](FloatLiteral &fL) {
                                 inst.s_Operand = varDec.s_Name;
                                 inst.s_Operation = std::to_string(fL.s_Value);
                               },
                               [&](StringLiteral &sL) {
                                 inst.s_Operand = varDec.s_Name;
                                 inst.s_Operation = sL.s_Value;
                               },
                               [&](BooleanLiteral &bL) {
                                 inst.s_Operand = varDec.s_Name;
                                 inst.s_Operation = std::to_string(bL.s_Value);
                               },
                           },
                           varDec.s_Initializer);
              },
              [&](ReturnStatement &retStmt) {
                inst.s_Operand = "RET";
                std::visit(U{
                               [&](IntegerLiteral &iL) {
                                 inst.s_Operation = std::to_string(iL.s_Value);
                               },
                               [&](FloatLiteral &fL) {
                                 inst.s_Operation = std::to_string(fL.s_Value);
                               },
                               [&](StringLiteral &sL) {
                                 inst.s_Operation = sL.s_Value;
                               },
                               [&](BooleanLiteral &bL) {
                                 inst.s_Operation = std::to_string(bL.s_Value);
                               },
                           },
                           retStmt.s_Expression);
              },
          },
          stmt);
      instruction.push_back(inst);
    }

    return instruction;
  };
};
