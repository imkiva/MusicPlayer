#include "koll/Text.h"

using namespace koll;

const char* Text::toCString(bool b)
{
	return b ? "true" : "false";
}

int Text::getCharWidth(const char &ch)
{
	if (ch < 0x80) {
		return 1;
	}
	return 2;
}

int Text::getCharByteOffset(const char &ch)
{
	if (ch < 0x80) {
		return 1;
	} else if
	((ch & 0b11111100) == 0b11111100) {
		return 6;
	} else if
	((ch & 0b11111000) == 0b11111000) {
		return 5;
	} else if
	((ch & 0b11110000) == 0b11110000) {
		return 4;
	} else if
	((ch & 0b11100000) == 0b11100000) {
		return 3;
	} else if
	((ch & 0b11000000) == 0b11000000) {
		return 2;
	}
}

int Text::getStringWidth(const std::string &str)
{
	int width = 0;
	int size = str.size();

	for(int i=0; i<size; i+=Text::getCharByteOffset(str[i])) {
		width += Text::getCharWidth(str[i]);
	}

	return width;
}

int Text::getPartString(const std::string &str, int width)
{
	int cwidth = 0;
	int pos = 0;
	int size = str.size();

	for(pos=0; pos<size; pos+=Text::getCharByteOffset(str[pos])) {
		if(cwidth+Text::getCharWidth(str[pos]) > width) {
			break;
		}
		cwidth += Text::getCharWidth(str[pos]);
	}

	return pos;
}

