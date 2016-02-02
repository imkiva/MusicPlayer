#pragma once

#include <string>

#include "Lyric.h"

namespace kiva {

class MusicEntry
{
private:
	std::string name;
	std::string artist;
	std::string path;
	std::string lyricPath;
	Lyric lyric;
	
	void parseFileName(const std::string &name);
	
public:
	MusicEntry(const std::string &path);
	~MusicEntry();
	
	void loadLyric(const std::string &file);
	
	const std::string& getPath() const;
	const std::string& getName() const;
	const std::string& getArtist() const;
	const std::string& getLyricPath() const;
	
	const Lyric& getLyric() const;
};

}
