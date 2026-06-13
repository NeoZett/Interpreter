#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <memory>
#include <optional>

class Source
{
private:
	std::string text;

public:
	Source(std::string text);

	static Source fromFile(const std::filesystem::path& path);

	static Source fromString(const std::string& text);
	
	const std::string& str() const
	{
		return text;
	}

	char at(std::size_t index) const
	{
		return text.at(index);
	}

	std::size_t size() const
	{
		return text.size();
	}

	bool empty() const
	{
		return text.empty();
	}
};

struct SourceLocation
{
	std::size_t line = 1;
	std::size_t column = 1;
};

template <typename TokenType>
requires std::is_enum_v<TokenType>
struct Token
{
	TokenType type;
	std::string lexeme;
	SourceLocation location;
};

template <typename TokenType>
requires std::is_enum_v<TokenType>
class Lexer;

template <typename TokenType>
requires std::is_enum_v<TokenType>
struct Rule
{
	using LexerT = Lexer<TokenType>;
	using TokenT = Token<TokenType>;

	virtual std::optional<TokenT>
		tryConsume(LexerT& lexer) = 0;

	virtual ~Rule() = default;
};

template <typename TokenType>
requires std::is_enum_v<TokenType>
class Lexer
{
private:
	using RulePtr = std::unique_ptr<Rule<TokenType>>;

	const Source* source;
	std::vector<RulePtr> rules;
	std::size_t index;
	std::size_t line;
	std::size_t column;

public:
	using TokenT = Token<TokenType>;

	Lexer(const Source& source, std::vector<RulePtr> rules)
		: source(&source), rules(std::move(rules)), index(0), line(1), column(1)
	{
	}

	bool isAtEnd() const
	{
		return index >= source->size();
	}

	char peek(std::size_t offset = 0) const
	{
		std::size_t pos = index + offset;

		if (pos >= source->size())
			return '\0';

		return source->at(pos);
	}

	char advance()
	{
		if (isAtEnd())
			return '\0';

		char c = source->at(index++);

		if (c == '\n')
		{
			++line;
			column = 1;
		}
		else
		{
			++column;
		}

		return c;
	}

	SourceLocation currentLocation() const
	{
		return { line, column };
	}

	bool match(char expected)
	{
		if (isAtEnd()) return false;
		if (source->at(index) != expected) return false;

		advance();
		return true;
	}

	std::size_t getIndex() const
	{
		return index;
	}

	std::string substrFrom(std::size_t start) const
	{
		return source->str().substr(start, index - start);
	}

	TokenT nextToken()
	{
		if (isAtEnd())
		{
			return TokenT{ TokenType::EndOfFile, "", currentLocation() };
		}

		for (auto& rule : rules)
		{
			if (auto token = rule->tryConsume(*this))
				return *token;
		}

		throw std::runtime_error(
			"Unexpected character: " + 
			std::string(1, peek())
		);
	}

	std::vector<TokenT> tokenize()
	{
		std::vector<TokenT> tokens;

		while (true)
		{
			TokenT token = nextToken();
			tokens.push_back(token);

			if (token.type == TokenType::EndOfFile)
				break;
		}

		return tokens;
	}
};