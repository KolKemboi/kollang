#pragma once

#include <cctype>
#include <cstddef>
#include <cstdio>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
enum class TokenType {
  RETURN,      // ret
  INT_T,       // int -> type
  INT_V,       // int -> type
  FLOAT_T,     // float-> value
  FLOAT_V,     // float-> value
  SEMICOLON,   // command end
  IDENTIFIER,  // name of var
  WHITESPACE,  // space
  END_OF_FILE, // eof
  ERROR,       // dunno this yet
  STR_T,       // str -> value
  STR_V,       // str -> value
  EQUAL,       // =
  RBRACE,      //{
  LBRACE,      //}
  RPAREN,      //(
  LPAREN,      //)
  BOOL,        // bool -> type/val
  MAIN,        //  the main keyword
  TRUE,
  FALSE,
};

inline const std::unordered_map<std::string_view, TokenType> keywords = {
    {"ret", TokenType::RETURN},  {"int", TokenType::INT_T},
    {"str", TokenType::STR_T},   {"{", TokenType::LBRACE},
    {"}", TokenType::RBRACE},    {"=", TokenType::EQUAL},
    {"true", TokenType::TRUE},   {"false", TokenType::FALSE},
    {"bul", TokenType::BOOL},    {"main", TokenType::MAIN},
    {"(", TokenType::LPAREN},    {")", TokenType::RPAREN},
    {"flt", TokenType::FLOAT_T},

};

struct Token {
  TokenType s_Type;         // what tok type
  std::string_view s_Value; // what is stored
  size_t s_Line;            // where it is in x and y
  size_t s_Column;

  Token(TokenType type = TokenType::END_OF_FILE, std::string_view value = "",
        size_t line = 1, size_t column = 1)
      : s_Type(type), s_Value(value), s_Line(line), s_Column(column) {}
};

class Lexer {
public:
  explicit Lexer(std::string_view &SourceCode) : m_SourceCode(SourceCode) {};

  std::optional<Token> nextToken() {
    _skipWhitespace();
    char c = this->_peek();

    if (c == '\0') {
      return Token(TokenType::END_OF_FILE, "", this->m_Line, this->m_Column);
    }
    if (c == ';') {
      this->_advance();
      return Token(TokenType::SEMICOLON, ";", this->m_Line, this->m_Column - 1);
    }
    if (c == '(') {
      this->_advance();
      return Token(TokenType::LPAREN, "(", this->m_Line, this->m_Column - 1);
    }
    if (c == ')') {
      this->_advance();
      return Token(TokenType::RPAREN, ")", this->m_Line, this->m_Column - 1);
    }
    if (c == '{') {
      this->_advance();
      return Token(TokenType::LBRACE, "{", this->m_Line, this->m_Column - 1);
    }
    if (c == '}') {
      this->_advance();
      return Token(TokenType::RBRACE, "}", this->m_Line, this->m_Column - 1);
    }
    if (c == '=') {
      this->_advance();
      return Token(TokenType::EQUAL, "=", this->m_Line, this->m_Column - 1);
    }
    // if " read until "
    if (c == '"') {
      return this->_getString();
    }
    if (std::isdigit(c)) {
      return this->_readNumber();
    }
    if (std::isalpha(c) || c == '_') {
      return this->_readWord();
    }

    this->_advance();
    return Token{TokenType::ERROR, std::string_view(&c, 1), m_Line,
                 m_Column - 1}; // silence this thing kwanza
  }

  std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    while (std::optional<Token> token = nextToken()) {
      tokens.push_back(*token);
      if (token->s_Type == TokenType::END_OF_FILE) {
        break;
      }
    }
    return tokens;
  }

private:
  std::string_view m_SourceCode;
  size_t m_Position{0};
  size_t m_Line{1};
  size_t m_Column{1};

private:
  char _peek(size_t offset = 0) const { // just returns what is at pos
    size_t idx = m_Position + offset;
    if (idx < m_SourceCode.length())
      return m_SourceCode[idx];
    else
      return '\0';
  }

  char _advance() { // decides what to do next
    char c = this->_peek();
    if (c == '\n') {
      this->m_Line++;
      this->m_Column = 1;
    } else
      this->m_Column++;
    this->m_Position++;
    return c;
  }

  void _skipWhitespace() {
    while (std::isspace(this->_peek()))
      this->_advance();
  }

  Token _readNumber() {
    size_t start = this->m_Position;
    bool isFloat = false;

    // unsigned int dotCount = 0;

    // for floats, I can add an extra check for a period(.)
    while (std::isdigit(_peek())) {
      this->_advance();
    }
    if (_peek() == '.') {
      isFloat = true;
      _advance();
      while (std::isdigit(_peek())) {
        _advance();
      }
    }
    std::string_view num = m_SourceCode.substr(start, m_Position - start);

    if (isFloat) {
      return Token(TokenType::FLOAT_V, num, m_Line, m_Column - num.length()

      );
    }
    return Token(TokenType::INT_V, num, m_Line, m_Column - num.length());
  }

  Token _getString() {
    size_t start = m_Position + 1; // move one pos
    _advance();

    while (_peek() != '"' && _peek() != '\0') {
      _advance();
    }

    if (_peek() == '\0') {
      return Token(TokenType::ERROR, "unterminated string", m_Line, m_Column);
    }

    std::string_view word = m_SourceCode.substr(start, m_Position - start);

    _advance();

    return Token(TokenType::STR_V, word, m_Line, m_Column - word.length());
  }

  Token _readWord() {
    size_t start = m_Position;
    // "m31" => should be valid
    // "31m" => should be invalid
    while (std::isalnum(_peek()) || this->_peek() == '_') {
      this->_advance();
    }

    std::string_view word =
        this->m_SourceCode.substr(start, m_Position - start);

    std::unordered_map<std::string_view, TokenType>::const_iterator it =
        keywords.find(word);
    if (it != keywords.end()) {
      return Token(it->second, word, m_Line, m_Column - word.length());
    }
    return Token(TokenType::IDENTIFIER, word, m_Line, m_Column - word.length());
  }
};

inline void PrintTokens(std::vector<Token> &tokens) {
  for (auto token : tokens) {
    std::string out =
        " Token Value => " + std::string(token.s_Value) +
        " Token found in Line => " + std::to_string(token.s_Line) +
        " Token found in column => " + std::to_string(token.s_Column);
    if (token.s_Type == TokenType::IDENTIFIER) {
      out = "IDENTIFIER " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::SEMICOLON) {
      out = "SEMICOLON " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::RETURN) {
      out = "RETURN " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::END_OF_FILE) {
      out = "EOF " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::EQUAL) {
      out = "EQUAL " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::MAIN) {
      out = "MAIN " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::LPAREN) {
      out = "LPAREN " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::RPAREN) {
      out = "RPAREN " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::LBRACE) {
      out = "LBRACE " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::RBRACE) {
      out = "RBRACE " + out;
      printf("%s\n", out.c_str());
    }

    if (token.s_Type == TokenType::INT_T) {
      out = "INT_T " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::INT_V) {
      out = "INT_V " + out;
      printf("%s\n", out.c_str());
    }

    if (token.s_Type == TokenType::STR_T) {
      out = "STR_T " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::STR_V) {
      out = "STR_V " + out;
      printf("%s\n", out.c_str());
    }

    if (token.s_Type == TokenType::FLOAT_T) {
      out = "FLOAT_T " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::FLOAT_V) {
      out = "FLOAT_V " + out;
      printf("%s\n", out.c_str());
    }

    if (token.s_Type == TokenType::BOOL) {
      out = "BOOL " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::TRUE) {
      out = "TRUE " + out;
      printf("%s\n", out.c_str());
    }
    if (token.s_Type == TokenType::FALSE) {
      out = "FALSE " + out;
      printf("%s\n", out.c_str());
    }
  }
}
