#include "Lexer.hpp"

void Lexer::skipWhitespace()
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

bool Lexer::isAlpha(char c)
{
	return std::isalpha(static_cast<unsigned char>(c))
		|| c == '_';
}

bool Lexer::isDigit(char c)
{
	return std::isdigit(static_cast<unsigned char>(c));
}

bool Lexer::isAlphaNumeric(char c)
{
	return isAlpha(c) || isDigit(c);
}

Lexer::Lexer(const Source& source)
	: source(&source), index(0), line(1), column(1)
{
}

bool Lexer::isAtEnd() const
{
	return index >= source->size();
}

char Lexer::peek(std::size_t offset = 0) const
{
	std::size_t pos = index + offset;

	if (pos >= source->size())
		return '\0';

	return source->at(pos);
}

char Lexer::advance()
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

SourceLocation Lexer::currentLocation() const
{
	return { line, column };
}

bool Lexer::match(char expected)
{
	if (isAtEnd()) return false;
	if (source->at(index) != expected) return false;

	advance();
	return true;
}

std::size_t Lexer::getIndex() const
{
	return index;
}

std::string Lexer::substrFrom(std::size_t start) const
{
	return source->str().substr(start, index - start);
}

Token Lexer::identifier()
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

Token Lexer::number()
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

Token Lexer::makeSingle(TokenType type)
{
	SourceLocation start = currentLocation();

	char c = advance();

	return {
		type,
		std::string(1, c),
		start
	};
}

Token Lexer::nextToken()
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

std::vector<Token> Lexer::tokenize()
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