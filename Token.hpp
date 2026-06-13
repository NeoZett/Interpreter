#pragma once

#include <string>

enum class TokenType
{
	Identifier,
	Number,

	Plus,
	Minus,
	Star,
	Slash,

	LeftParen,
	RightParen,

	Equal,
	EqualEqual,

	Less,
	LessEqual,

	Greater,
	GreaterEqual,

	EndOfFile
};

struct SourceLocation
{
	std::size_t line = 1;
	std::size_t column = 1;
};

struct Token
{
	TokenType type;
	std::string lexeme;
	SourceLocation location;
};