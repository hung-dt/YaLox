#pragma once

#include "token.hpp"

#include <string>
#include <vector>

namespace lox {

/*---------------------------------------------------------------------------*/

class Scanner
{
public:
  Scanner(const std::string& source);

  std::vector<Token> scanTokens();

private:
  const std::string source_;
  std::vector<Token> tokens_;

  // keep track of where the scanner is in the source code
  size_t start_{};
  size_t current_{};
  int line_{ 1 };

  void scanToken();
  void scanString();
  void scanNumber();
  void scanIdentifier();
  bool match(char);
  char peek() const;
  char peekNext() const;
  void addToken(TokenType, const LoxObject& literal = std::nullopt);

  bool isAtEnd() const;
};

}
