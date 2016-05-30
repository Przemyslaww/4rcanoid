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

std::string toKeyCodeByte(const long long unsigned& keyCode) {
	if (keyCode == SDLK_LEFT) return charToOneCharString(LEFT_KEY);
	if (keyCode == SDLK_RIGHT) return charToOneCharString(RIGHT_KEY);
	if (keyCode == SDLK_UP) return charToOneCharString(UP_KEY);
	if (keyCode == SDLK_DOWN) return charToOneCharString(DOWN_KEY);
	if (keyCode == SDLK_ESCAPE) return charToOneCharString(ESCAPE_KEY);
	if (keyCode == SDLK_SPACE) return charToOneCharString(SPACE_KEY);
	return "" + 255;
}

std::string charToOneCharString(const char& arg) {
	return std::string(1, arg);
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