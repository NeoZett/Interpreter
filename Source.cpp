#include "Source.hpp"

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

const std::string& Source::str() const
{
	return text;
}

char Source::at(std::size_t index) const
{
	return text.at(index);
}

std::size_t Source::size() const
{
	return text.size();
}

bool Source::empty() const
{
	return text.empty();
}