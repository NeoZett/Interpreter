#pragma once

#include <memory>
#include <vector>
#include <map>

#include "Token.hpp"

struct ASTNode
{
    SourceLocation location;

    explicit ASTNode(SourceLocation location)
        : location(location)
    {
    }

    virtual ~ASTNode() = default;
};

struct Expression : ASTNode
{
    explicit Expression(SourceLocation location)
        : ASTNode(location)
    {
    }
};

struct LiteralExpression : Expression
{
    Token value;

    explicit LiteralExpression(Token value)
		: value(std::move(value)), 
        Expression(value.location)
    {
    }
};

struct IdentifierExpression : Expression
{
    Token name;

    explicit IdentifierExpression(Token name)
		: name(std::move(name)), 
        Expression(name.location)
    {
    }
};

struct BinaryExpression : Expression
{
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;

    BinaryExpression(
        std::unique_ptr<Expression> left,
        Token op,
        std::unique_ptr<Expression> right)
        : left(std::move(left)),
        op(std::move(op)),
        right(std::move(right)),
		Expression(op.location)
    {
    }
};

struct UnaryExpression : Expression
{
    Token op;
    std::unique_ptr<Expression> operand;

    UnaryExpression(
        Token op,
        std::unique_ptr<Expression> operand)
        : op(std::move(op)),
        operand(std::move(operand)),
		Expression(op.location)
    {
    }
};

struct AssignmentExpression : Expression
{
    Token name;
    std::unique_ptr<Expression> value;

    AssignmentExpression(
        Token name,
        std::unique_ptr<Expression> value)
        : name(std::move(name)),
        value(std::move(value)),
		Expression(name.location)
    {
    }
};

struct GroupingExpression : Expression
{
    std::unique_ptr<Expression> expression;

    explicit GroupingExpression(
        std::unique_ptr<Expression> expression)
        : expression(std::move(expression)),
		Expression(expression->location)
    {
    }
};

struct AccessExpression : Expression
{
    std::unique_ptr<Expression> object;
    Token member;

    explicit AccessExpression(
        std::unique_ptr<Expression> object,
        Token member)
        : object(std::move(object)),
        member(std::move(member)),
        Expression(member.location)
    {
    }
};

struct MemberAssignmentExpression : Expression
{
    std::unique_ptr<Expression> object;
    Token member;
    std::unique_ptr<Expression> value;

    MemberAssignmentExpression(
        std::unique_ptr<Expression> object,
        Token member,
        std::unique_ptr<Expression> value)
        : object(std::move(object)),
        member(std::move(member)),
        value(std::move(value)),
        Expression(member.location)
    {
    }
};

struct CallExpression : Expression
{
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;

    CallExpression(
        std::unique_ptr<Expression> callee,
        std::vector<std::unique_ptr<Expression>> arguments)
        : callee(std::move(callee)),
        arguments(std::move(arguments)),
        Expression(callee->location)
    {
    }
};

struct Statement : ASTNode
{
    explicit Statement(SourceLocation location)
        : ASTNode(location)
    {
    }
};

struct ExpressionStatement : Statement
{
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(
        std::unique_ptr<Expression> expression)
        : expression(std::move(expression)),
		Statement(expression->location)
    {
    }
};

struct VariableDeclaration : Statement
{
    Token name;
    std::unique_ptr<Expression> initializer;

    VariableDeclaration(
        Token name,
        std::unique_ptr<Expression> initializer)
        : name(std::move(name)),
        initializer(std::move(initializer)),
		Statement(name.location)
    {
    }
};

struct FunctionDeclaration : Statement
{
    Token name;
    std::vector<Token> parameters;
    std::vector<std::unique_ptr<Statement>> body;

    FunctionDeclaration(
        Token name,
        std::vector<Token> parameters,
        std::vector<std::unique_ptr<Statement>> body)
        : name(std::move(name)),
        parameters(std::move(parameters)),
        body(std::move(body)),
        Statement(name.location)
    {
    }
};

struct ClassDeclaration : Statement
{
    Token name;
    std::vector<std::unique_ptr<FunctionDeclaration>> methods;

    ClassDeclaration(
        Token name,
        std::vector<std::unique_ptr<FunctionDeclaration>> methods)
        : name(std::move(name)),
        methods(std::move(methods)),
        Statement(name.location)
    {
    }
};

struct ProgramMetadata
{
    std::string sourcePath;
};

struct Program : ASTNode
{
	// Attributes is where we can define the path to the source file, and any other metadata we want to associate with the program.
	ProgramMetadata metadata;
    std::vector<std::unique_ptr<Statement>> statements;

    Program() : ASTNode(SourceLocation{}) {}
};