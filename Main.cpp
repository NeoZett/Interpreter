#include <iostream>

#include "Parser"

int main()
{
	std::string sourceCode = "x = 5 + 3 * (2 - 1)";
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
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Parsing error: " << e.what() << std::endl;
	}
	return 0;
}