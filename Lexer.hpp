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

	EndOfFile
};

struct Token
{
	TokenType type;
	std::string lexeme;
	SourceLocation location;
};

class Lexer
{
private:
	const Source* source;
	std::size_t index;
	std::size_t line;
	std::size_t column;

	void skipWhitespace()
	{
		while (!isAtEnd())
		{
			char c = peek();

			switch (c)
			{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				advance();
				break;

			default:
				return;
			}
		}
	}

	static bool isAlpha(char c)
	{
		return std::isalpha(static_cast<unsigned char>(c))
			|| c == '_';
	}

	static bool isDigit(char c)
	{
		return std::isdigit(static_cast<unsigned char>(c));
	}

	static bool isAlphaNumeric(char c)
	{
		return isAlpha(c) || isDigit(c);
	}

public:
	Lexer(const Source& source)
		: source(&source), index(0), line(1), column(1)
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

	Token identifier()
	{
		SourceLocation start = currentLocation();
		std::size_t begin = index;

		while (isAlphaNumeric(peek()))
			advance();

		return {
			TokenType::Identifier,
			substrFrom(begin),
			start
		};
	}

	Token number()
	{
		SourceLocation start = currentLocation();
		std::size_t begin = index;

		while (isDigit(peek()))
			advance();

		return {
			TokenType::Number,
			substrFrom(begin),
			start
		};
	}

	Token makeSingle(TokenType type)
	{
		SourceLocation start = currentLocation();

		char c = advance();

		return {
			type,
			std::string(1, c),
			start
		};
	}

	Token nextToken()
	{
		skipWhitespace();

		if (isAtEnd())
		{
			return {
				TokenType::EndOfFile,
				"",
				currentLocation()
			};
		}

		char c = peek();

		if (isAlpha(c))
			return identifier();

		if (isDigit(c))
			return number();

		switch (c)
		{
		case '+':
			return makeSingle(TokenType::Plus);

		case '-':
			return makeSingle(TokenType::Minus);

		case '*':
			return makeSingle(TokenType::Star);

		case '/':
			return makeSingle(TokenType::Slash);

		case '(':
			return makeSingle(TokenType::LeftParen);

		case ')':
			return makeSingle(TokenType::RightParen);

		case '=':
		{
			SourceLocation start = currentLocation();

			advance();

			if (match('='))
			{
				return {
					TokenType::EqualEqual,
					"==",
					start
				};
			}

			return {
				TokenType::Equal,
				"=",
				start
			};
		}

		default:
			throw std::runtime_error(
				"Unexpected character: " +
				std::string(1, c)
			);
		}
	}

	std::vector<Token> tokenize()
	{
		std::vector<Token> tokens;

		while (true)
		{
			Token token = nextToken();
			tokens.push_back(token);

			if (token.type == TokenType::EndOfFile)
				break;
		}

		return tokens;
	}
};