#pragma once
#include "Lexer.hpp"
#include <string>
#include <variant>
#include <vector>

// EXPRESSIONS
struct IntegerLiteral {
  int s_Value;
};

struct FloatLiteral {
  float s_Value;
};

struct StringLiteral {
  std::string s_Value;
};

struct BooleanLiteral {
  bool s_Value;
};

struct VariableExpression {
  std::string s_Name;
};

using Expression = std::variant<IntegerLiteral, FloatLiteral, StringLiteral,
                                BooleanLiteral, VariableExpression>;

// STATEMENTS

struct VariableDeclaration {
  TokenType s_Type;
  std::string s_Name;
  Expression s_Initializer;
};

struct ReturnStatement {
  Expression s_Expression;
};

using Statement = std::variant<VariableDeclaration, ReturnStatement>;

// Program

struct MainFunction {
  std::vector<Statement> s_Statements;
};

struct Program {
  MainFunction s_Main;
};
