#pragma once

#include "Lexer.hpp"

template <typename TokenType>
requires std::is_enum_v<TokenType>
class SingleCharRule : public Rule<TokenType>
{
private:
	using RuleType = Rule<TokenType>;

	char expected;
	TokenType type;

public:
	SingleCharRule(char expected, TokenType type)
		: expected(expected), type(type)
	{
	}

	std::optional<typename RuleType::TokenT>
		tryConsume(typename RuleType::LexerT& lexer) override
	{
		if (lexer.peek() != expected)
			return std::nullopt;

		lexer.advance();

		return RuleType::TokenT{
			type,
			std::string(1, expected),
			lexer.currentLocation()
		};
	}
};

template <typename TokenType>
requires std::is_enum_v<TokenType>
class MultiCharRule : public Rule<TokenType>
{
private:
	using RuleType = Rule<TokenType>;

	std::string expected;
	TokenType type;

public:
	SingleCharRule(std::string expected, TokenType type)
		: expected(expected), type(type)
	{
	}

	std::optional<typename RuleType::TokenT>
		tryConsume(typename RuleType::LexerT& lexer) override
	{
		for (size_t i = 0; i < expected.size(); i++)
		{
			if (lexer.peek(i) != expected[i])
				return std::nullopt;
		}

		for (size_t i = 0; i < expected.size(); i++)
			lexer.advance();

		return TokenT{
			type,
			expected,
			lexer.currentLocation()
		};
	}
};

template <typename TokenType>
requires std::is_enum_v<TokenType>
class PatternRule : public Rule<TokenType>
{
private:
	using RuleType = Rule<TokenType>;

	TokenType type;
	std::function<bool(char)> start;
	std::function<bool(char)> cont;

public:
	PatternRule(
		TokenType type,
		std::function<bool(char)> start,
		std::function<bool(char)> cont)
		: expected(expected), type(type)
	{
	}

	std::optional<typename RuleType::TokenT>
		tryConsume(typename RuleType::LexerT& lexer) override
	{
		if (!start(lexer.peek()))
			return std::nullopt;

		std::size_t startIndex = lexer.getIndex(); // you may need accessor

		lexer.advance();

		while (cont(lexer.peek()))
			lexer.advance();

		std::string lexeme = lexer.substrFrom(startIndex);

		return TokenT{
			type,
			lexeme,
			lexer.currentLocation()
		};
	}
};

namespace LexerFactory
{
	template <typename TokenType>
	requires std::is_enum_v<TokenType>
	static Lexer<TokenType> create(const Source& source);
};