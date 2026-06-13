#pragma once


#include "Source.hpp"
#include "Token.hpp"
#include "Lexer.hpp"

class Parser
{
private:
	Lexer lexer;
	std::vector<Token> tokens;
	std::size_t current;

public:
	Parser(const Source& source);
	bool isAtEnd() const;
	const Token& peek(std::size_t offset = 0) const;
	const Token& advance();
	bool match(TokenType expected);
	void expect(TokenType expected);
	void optionallyExpect(TokenType expected);
	void parse();
};