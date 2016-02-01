#pragma once

#include "koll/Koll.h"

namespace koll {

class Text {
private:
	Text() {}
	~Text() {}

public:
	static const char* toCString(bool b);
	
	static int getCharWidth(const char &ch);
	
	static int getCharByteOffset(const char &ch);
	
	static int getStringWidth(const std::string &str);
	
	static int getPartString(const std::string &str, int width);
};


}

