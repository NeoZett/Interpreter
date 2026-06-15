#include <iostream>

#include "Parser"

static void printInfoExpression(const std::unique_ptr<Expression>& expression)
{
	if (auto literalExpr = dynamic_cast<LiteralExpression*>(expression.get()))
	{
		std::cout << "Literal expression with value '" << literalExpr->value.lexeme
			<< "' at line " << literalExpr->location.line
			<< ", column " << literalExpr->location.column << std::endl;
	}
	else if (auto identifierExpr = dynamic_cast<IdentifierExpression*>(expression.get()))
	{
		std::cout << "Identifier expression with name '" << identifierExpr->name.lexeme
			<< "' at line " << identifierExpr->location.line
			<< ", column " << identifierExpr->location.column << std::endl;
	}
	else if (auto binaryExpr = dynamic_cast<BinaryExpression*>(expression.get()))
	{
		std::cout << "Binary expression with operator '" << binaryExpr->op.lexeme
			<< "' at line " << binaryExpr->location.line
			<< ", column " << binaryExpr->location.column << std::endl;
	}
	else if (auto unaryExpr = dynamic_cast<UnaryExpression*>(expression.get()))
	{
		std::cout << "Unary expression with operator '" << unaryExpr->op.lexeme
			<< "' at line " << unaryExpr->location.line
			<< ", column " << unaryExpr->location.column << std::endl;
	}
	else if (auto groupingExpr = dynamic_cast<GroupingExpression*>(expression.get()))
	{
		std::cout << "Grouping expression at line " << groupingExpr->location.line
			<< ", column " << groupingExpr->location.column << std::endl;
	}
	else
	{
		std::cout << "Unknown expression type at line " << expression->location.line
			<< ", column " << expression->location.column << std::endl;
	}
}

static void printInfoStatement(const std::unique_ptr<Statement>& statement)
{
	if (auto exprStmt = dynamic_cast<ExpressionStatement*>(statement.get()))
	{
		std::cout << "Expression statement at line " << exprStmt->location.line
			<< ", column " << exprStmt->location.column << std::endl;
		printInfoExpression(exprStmt->expression);
	}
	else if (auto assignExpr = dynamic_cast<AssignmentExpression*>(statement.get()))
	{
		std::cout << "Assignment expression at line " << assignExpr->location.line
			<< ", column " << assignExpr->location.column << std::endl;
		printInfoExpression(assignExpr->value);
	}
	else if (auto varStmt = dynamic_cast<VariableDeclaration*>(statement.get()))
	{
		std::cout << "Variable declaration at line " << varStmt->location.line
			<< ", column " << varStmt->location.column << " with name " << varStmt->name.lexeme << std::endl;
	}
	else
	{
		std::cout << "Unknown statement type at line " << statement->location.line
			<< ", column " << statement->location.column << std::endl;
	}
}

int main()
{
	std::string sourceCode = "x = 5 + 3 * (2 - 1);";
	Source source(sourceCode);
	Parser parser(source);
	try
	{
		Program program = parser.parse();
		std::cout << "Parsing successful!" << std::endl;

		for (const auto& statement : program.statements)
		{
			// Here you would typically have a function to print or evaluate the AST.
			// For demonstration, we'll just print the type of statement.
			std::cout << "Parsed a statement at line " << statement->location.line
				<< ", column " << statement->location.column << std::endl;

			printInfoStatement(statement);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Parsing error: " << e.what() << std::endl;
	}
	return 0;
}