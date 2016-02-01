#pragma once

#include <string>

#include "Lyric.h"

namespace kiva {

class MusicEntry {
private:
	std::string name;
	std::string artist;
	std::string path;
	Lyric lyric;
	
	void parseFileName(const std::string &name);
	
public:
	MusicEntry(const std::string &path);
	~MusicEntry();
	
	const std::string& getPath() const;
	const std::string& getName() const;
	const std::string& getArtist() const;
	const Lyric& getLyric() const;
};

}
