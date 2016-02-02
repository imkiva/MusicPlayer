#pragma once

#include <vector>
#include <string>

#include "MusicPlayer.h"

namespace kiva {

typedef Millisecond LyricTime;

typedef struct
{
	std::string lyric;
	LyricTime startTime;
	LyricTime endTime;
} LyricItem;

class Lyric
{
private:
	std::string ar;
	std::string ti;
	std::vector<LyricItem> data;

	void parseLine(const std::string &line);
	void parseHeader(const std::string &tag);
	LyricTime parseTime(const std::string &tag);

	/* 解析出的data是有序的， 所以可以使用二分查找 */
	int binarySearchLyric(LyricTime tm) const;
	bool inRange(LyricTime start, LyricTime end, LyricTime tm) const;

public:
	Lyric();
	~Lyric();
	
	void clear();
	bool parseFile(const std::string &file);

	const std::string& getLyric(LyricTime tm, int *index = NULL) const;
	const std::string& getLyricAt(int index) const;
	
	const std::string& getArtist() const;
	const std::string& getTitle() const;
	
	bool hasData() const;
};

}
