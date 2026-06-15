#include "Parser.hpp"

std::unique_ptr<Statement> Parser::parseStatement()
{
	if (check(TokenType::Function))
		return parseFunction();

	if (check(TokenType::Class))
		return parseClass();

	return std::make_unique<ExpressionStatement>(
		parseExpression()
	);
}

std::unique_ptr<Expression> Parser::parseExpression()
{
	return parseAssignment();
}

std::unique_ptr<Expression> Parser::parseAssignment()
{
	auto expr = parseEquality();

	if (match(TokenType::Equal))
	{
		Token equals = previous();

		auto value = parseAssignment();

		auto* identifier =
			dynamic_cast<IdentifierExpression*>(expr.get());

		if (!identifier)
		{
			throw std::runtime_error(
				"Invalid assignment target."
			);
		}

		return std::make_unique<AssignmentExpression>(
			identifier->name,
			std::move(value)
		);
	}

	return expr;
}

std::unique_ptr<Expression> Parser::parseEquality()
{
	auto expr = parseComparison();

	while (match(TokenType::EqualEqual))
	{
		Token op = previous();

		auto right = parseTerm();

		expr = std::make_unique<BinaryExpression>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}

std::unique_ptr<Expression> Parser::parseComparison()
{
	auto expr = parseTerm();

	while (
		match(TokenType::Less) ||
		match(TokenType::LessEqual) ||
		match(TokenType::Greater) ||
		match(TokenType::GreaterEqual)
		)
	{
		Token op = previous();

		auto right = parseTerm();

		expr = std::make_unique<BinaryExpression>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}

std::unique_ptr<Expression> Parser::parseTerm()
{
	auto expr = parseFactor();

	while (
		match(TokenType::Plus) ||
		match(TokenType::Minus)
		)
	{
		Token op = previous();

		auto right = parseFactor();

		expr = std::make_unique<BinaryExpression>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}

std::unique_ptr<Expression> Parser::parseFactor()
{
	auto expr = parseUnary();

	while (
		match(TokenType::Star) ||
		match(TokenType::Slash)
		)
	{
		Token op = previous();

		auto right = parseUnary();

		expr = std::make_unique<BinaryExpression>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}

std::unique_ptr<Expression> Parser::parseUnary()
{
	if (match(TokenType::Minus))
	{
		Token op = previous();

		auto operand = parseUnary();

		return std::make_unique<UnaryExpression>(
			op,
			std::move(operand)
		);
	}

	return parsePostfix();
}

std::unique_ptr<Expression> Parser::parsePrimary()
{
	if (match(TokenType::Number))
	{
		return std::make_unique<LiteralExpression>(
			previous()
		);
	}

	if (match(TokenType::Identifier))
	{
		return std::make_unique<IdentifierExpression>(
			previous()
		);
	}

	if (match(TokenType::LeftParen))
	{
		auto expr = parseExpression();

		expect(TokenType::RightParen);

		return std::make_unique<GroupingExpression>(
			std::move(expr)
		);
	}

	throw std::runtime_error(
		"Expected expression."
	);
}

std::unique_ptr<Expression> Parser::parsePostfix()
{
	auto expr = parsePrimary();

	while (true)
	{
		if (match(TokenType::Dot))
		{
			Token member = peek();

			expect(TokenType::Identifier);

			expr = std::make_unique<AccessExpression>(
				std::move(expr),
				member
			);
		}
		else if (match(TokenType::LeftParen))
		{
			std::vector<std::unique_ptr<Expression>> args;

			if (!check(TokenType::RightParen))
			{
				do
				{
					args.push_back(parseExpression());
				} while (match(TokenType::Comma));
			}

			expect(TokenType::RightParen);

			expr = std::make_unique<CallExpression>(
				std::move(expr),
				std::move(args)
			);
		}
		else
		{
			break;
		}
	}

	return expr;
}

std::unique_ptr<FunctionDeclaration> Parser::parseFunction()
{
	expect(TokenType::Function);

	Token name = peek();
	expect(TokenType::Identifier);

	expect(TokenType::LeftParen);

	std::vector<Token> parameters;

	if (!check(TokenType::RightParen))
	{
		do
		{
			Token param = peek();
			expect(TokenType::Identifier);

			parameters.push_back(param);
		} while (match(TokenType::Comma));
	}

	expect(TokenType::RightParen);

	expect(TokenType::LeftBrace);

	std::vector<std::unique_ptr<Statement>> body;

	while (!check(TokenType::RightBrace))
	{
		body.push_back(parseStatement());
		match(TokenType::Semicolon);
	}

	expect(TokenType::RightBrace);

	return std::make_unique<FunctionDeclaration>(
		std::move(name),
		std::move(parameters),
		std::move(body)
	);
}

std::unique_ptr<ClassDeclaration> Parser::parseClass()
{
	expect(TokenType::Class);

	Token name = peek();
	expect(TokenType::Identifier);
	
	expect(TokenType::LeftBrace);

	std::vector<std::unique_ptr<FunctionDeclaration>>
		methods;

	while (!check(TokenType::RightBrace))
	{
		methods.push_back(parseFunction());
	}

	expect(TokenType::RightBrace);

	return std::make_unique<ClassDeclaration>(
		std::move(name),
		std::move(methods)
	);
}

Parser::Parser(const Source& source)
	: lexer(source), current(0)
{
	tokens = lexer.tokenize();
}

bool Parser::isAtEnd() const
{
	return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek(std::size_t offset) const
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
		match(TokenType::Semicolon);
	}

	return program;
}