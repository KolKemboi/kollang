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

struct Conditional {
  // name and a name
  // value and a value
  // name and a value
  // Tokentype::EQ
  TokenType s_Comparator;
  // name of an existing variable
  // or
  // an expression
  std::variant<std::string, Expression> s_Value_1;
  std::variant<std::string, Expression> s_Value_2;
};

using Statement = std::variant<VariableDeclaration, ReturnStatement>;

struct IfBlock {
  // IF LPAREN CONDITION RPAREN LBRACE RBRACE
  std::variant<BooleanLiteral, Conditional> s_Conditional;
  std::vector<Statement> s_Statements;
};

using ForExpression = std::variant<IntegerLiteral, FloatLiteral>;

struct ForInitialCondition {
  TokenType s_Type;
  std::string s_Name;
  ForExpression s_Initializer;
};

struct ForBoundaryCondition {
  std::string s_Name;
  TokenType s_Comparator;
  std::variant<std::string, ForExpression> s_Comparatee;

  ForBoundaryCondition(ForInitialCondition initCond)
      : s_Name(initCond.s_Name) {};
};

struct ForIncrement {
  std::string s_Name;
  TokenType s_BinaryOp;
  ForIncrement(ForInitialCondition initCond) : s_Name(initCond.s_Name) {}
};

struct ForLoop {
  // initial condtion std::var of float or int
  // boundary condition => conditional
  // increament/decrement operation
  // statements
  ForInitialCondition s_InitialCondition;
  ForBoundaryCondition s_BoundaryCondition;
  std::variant<ForIncrement> s_Increment;
  std::vector<Statement> s_Statements;
};

// Program

struct MainFunction {
  std::vector<Statement> s_Statements;
};

struct Program {
  MainFunction s_Main;
};
