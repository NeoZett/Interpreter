#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <memory>
#include <optional>

#include "Source.hpp"
#include "Token.hpp"

class Lexer
{
private:
	const Source* source;
	std::size_t index;
	std::size_t line;
	std::size_t column;

	void skipWhitespace();
	static bool isAlpha(char c);
	static bool isDigit(char c);
	static bool isAlphaNumeric(char c);

public:
	Lexer(const Source& source);
	bool isAtEnd() const;
	char peek(std::size_t offset = 0) const;
	char advance();
	SourceLocation currentLocation() const;
	bool match(char expected);
	std::size_t getIndex() const;
	std::string substrFrom(std::size_t start) const;
	Token identifier();
	Token number();
	Token makeSingle(TokenType type);
	Token nextToken();
	std::vector<Token> tokenize();
};