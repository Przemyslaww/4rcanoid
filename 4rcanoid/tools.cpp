#include "header.hpp"

double n_rand() {
	return rand() / RAND_MAX;
}

std::string intToStr(const int& arg) {
	std::ostringstream ss;
	ss << arg;
	std::string str = ss.str();
	return str;
}

std::string charToOneCharString(const char& arg) {
	return "" + arg;
}

std::vector<std::string> split(const std::string& astr, char c = ' ')
{
	std::vector<std::string> result;
	const char* str = astr.c_str();

	do
	{
		const char *begin = str;

		while (*str != c && *str)
			str++;

		result.push_back(std::string(begin, str));
	} while (0 != *str++);

	return result;
}