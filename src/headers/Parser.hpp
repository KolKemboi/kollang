#pragma once
#include "AST.hpp"
#include "Lexer.hpp"
#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

class Parser {
public:
  explicit Parser(const std::vector<Token> &tokens) : m_Tokens(tokens) {}

  Program parse() { return _parseProgram(); }

private:
  const std::vector<Token> &m_Tokens;
  size_t m_Position{0};

private:
  const Token &_peek() const { return m_Tokens[m_Position]; }
  const Token &_advance() { return m_Tokens[m_Position++]; }
  bool _check(TokenType type) const { return _peek().s_Type == type; }

  const Token &expect(TokenType type) {
    if (!_check(type))
      throw std::runtime_error("Unexpected token at line" +
                               std::to_string(_peek().s_Line));
    return _advance();
  }

  Program _parseProgram() {
    expect(TokenType::INT_T);
    expect(TokenType::MAIN);
    expect(TokenType::LPAREN);
    expect(TokenType::RPAREN);
    expect(TokenType::LBRACE);

    MainFunction main;

    while (!_check(TokenType::RBRACE) && !_check(TokenType::END_OF_FILE)) {
      main.s_Statements.push_back(_parseStatements());
    }
    expect(TokenType::RBRACE);
    expect(TokenType::END_OF_FILE);
    return Program{std::move(main)};
  }

  Statement _parseStatements() {
    if (_check(TokenType::RETURN)) {
      return _parseReturnStatement();
    }

    if (_check(TokenType::INT_T) || _check(TokenType::BOOL) ||
        _check(TokenType::FLOAT_T) || _check(TokenType::STR_T)) {
      return _parseVariableDeclaration();
    }

    throw std::runtime_error("Expected statement at line" +
                             std::to_string(_peek().s_Line));
  }

  VariableDeclaration _parseVariableDeclaration() {
    TokenType type = _advance().s_Type;

    const Token &name = expect(TokenType::IDENTIFIER);

    expect(TokenType::EQUAL);

    Expression initializer = _parseExpression();

    expect(TokenType::SEMICOLON);
    return VariableDeclaration{type, std::string(name.s_Value),
                               std::move(initializer)

    };
  }

  ReturnStatement _parseReturnStatement() {
    expect(TokenType::RETURN);
    Expression expr = _parseExpression();
    expect(TokenType::SEMICOLON);
    return ReturnStatement{std::move(expr)};
  }

  Expression _parseExpression() {
    const Token &token = _peek();
    switch (token.s_Type) {
    case TokenType::INT_V: {
      _advance();
      return IntegerLiteral{std::stoi(std::string(token.s_Value))};
    }
    case TokenType::FLOAT_V: {
      _advance();
      return FloatLiteral{std::stof(std::string(token.s_Value))};
    }
    case TokenType::STR_V: {
      _advance();
      return StringLiteral{std::string(token.s_Value)};
    }
    case TokenType::TRUE: {
      _advance();
      return BooleanLiteral{true};
    }
    case TokenType::FALSE: {
      _advance();
      return BooleanLiteral{false};
    }
    default:
      throw std::runtime_error("Expected expression at line" +
                               std::to_string(token.s_Line));
    }
  }
};
