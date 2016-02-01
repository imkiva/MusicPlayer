#include "MusicEntry.h"

using namespace kiva;

MusicEntry::MusicEntry(const std::string &path)
	:path(path)
{
	int pos = path.find_last_of('/');
	if (pos == std::string::npos) {
		parseFileName(path);
	} else {
		std::string name = path.substr(pos+1);
		parseFileName(name);
	}
	
	const std::string &lrc = path.substr(0, path.find_last_of('.') + 1) + "lrc";
	lyric.parseFile(lrc);
}

MusicEntry::~MusicEntry()
{
}


void MusicEntry::parseFileName(const std::string &name)
{
	int pos = name.find(" - ");
	
	if (pos == std::string::npos) {
		this->name = name;
		this->artist = std::string("unknown");
	} else {
		this->artist = name.substr(0, pos);
		this->name = name.substr(pos+3);
	}
	
	this->name = this->name.substr(0, this->name.rfind("."));
}


const std::string& MusicEntry::getPath() const
{
	return path;
}


const std::string& MusicEntry::getName() const
{
	return name;
}


const std::string& MusicEntry::getArtist() const
{
	return artist;
}


const Lyric& MusicEntry::getLyric() const
{
	return lyric;
}

