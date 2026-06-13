#include "Parser.hpp"



Parser::Parser(const Source& source)
	: lexer(source), current(0)
{
	tokens = lexer.tokenize();
}

bool Parser::isAtEnd() const
{
	return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek(std::size_t offset = 0) const
{
	std::size_t index = current + offset;

	if (index >= tokens.size())
		return tokens.back();

	return tokens[index];
}

const Token& Parser::previous() const
{
	return tokens[current - 1];
}

const Token& Parser::advance()
{
	if (!isAtEnd())
		++current;

	return previous();
}

bool Parser::check(TokenType type) const
{
	return !isAtEnd() && peek().type == type;
}

bool Parser::match(TokenType expected)
{
	if (peek().type == expected)
	{
		advance();
		return true;
	}
	return false;
}

void Parser::expect(TokenType expected)
{
	if (!match(expected))
	{
		const Token& token = peek();
		throw std::runtime_error(
			"Expected token of type " +
			std::to_string(static_cast<int>(expected)) +
			", but got " +
			std::to_string(static_cast<int>(token.type)) +
			" at line " +
			std::to_string(token.location.line) +
			", column " +
			std::to_string(token.location.column)
		);
	}
}

Program Parser::parse()
{
	Program program;

	while (!isAtEnd())
	{
		program.statements.push_back(parseStatement());
	}

	return program;
}