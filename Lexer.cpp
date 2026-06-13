#include "Lexer.hpp"

Source::Source(std::string text)
	: text(std::move(text))
{
}

Source Source::fromFile(const std::filesystem::path& path)
{
	std::fstream file(path);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + path.string());
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	return Source(content);
}

Source Source::fromString(const std::string& text)
{
	return Source(text);
}

