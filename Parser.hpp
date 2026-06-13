#pragma once

#include "Source.hpp"
#include "Token.hpp"
#include "Lexer.hpp"

#include "AST.hpp"

class Parser
{
private:
	Lexer lexer;
	std::vector<Token> tokens;
	std::size_t current;

	std::unique_ptr<Statement> parseStatement();
	std::unique_ptr<Expression> parseExpression();
	std::unique_ptr<Expression> parseAssignment();
	std::unique_ptr<Expression> parseEquality();
	std::unique_ptr<Expression> parseComparison();
	std::unique_ptr<Expression> parseTerm();
	std::unique_ptr<Expression> parseFactor();
	std::unique_ptr<Expression> parseUnary();
	std::unique_ptr<Expression> parsePrimary();

public:
	Parser(const Source& source);
	bool isAtEnd() const;
	const Token& peek(std::size_t offset = 0) const;
	const Token& previous() const;
	const Token& advance();
	bool check(TokenType type) const;
	bool match(TokenType expected);
	void expect(TokenType expected);
	Program parse();
};