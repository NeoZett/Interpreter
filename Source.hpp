#pragma once

#include <filesystem>
#include <fstream>
#include <string>

class Source
{
private:
	std::string text;

public:
	Source(std::string text);
	static Source fromFile(const std::filesystem::path& path);
	static Source fromString(const std::string& text);
	const std::string& str() const;
	char at(std::size_t index) const;
	std::size_t size() const;
	bool empty() const;
};