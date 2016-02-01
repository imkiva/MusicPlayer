#include <stdio.h>
#include <string.h>

#include <string>
#include <fstream>

#include "Lyric.h"

using namespace kiva;


static bool startsWith(const std::string &str, const std::string &sub)
{
	return strncmp(str.c_str(), sub.c_str(), sub.length()) == 0;
}

Lyric::Lyric()
{

}


Lyric::~Lyric()
{

}


void Lyric::clear()
{
	data.clear();
}


void Lyric::parseLine(const std::string &line)
{
	if (line[0] != '[') {
		return;
	}

	int lpos = line.find_first_of('[');
	int rpos = line.find_first_of(']');

	if (rpos == std::string::npos) {
		return;
	}

	const std::string &tag = line.substr(lpos+1, rpos-1);
	const std::string &content = line.substr(rpos+1);

	if (content.empty()) { /* is header */
		parseHeader(tag);

	} else { /* is lyric */
		LyricTime tm = parseTime(tag);

		LyricItem item;
		item.lyric = std::move(content[0] == ' ' ? content.substr(1) : content);
		item.startTime = tm;
		item.endTime = 0;

		int sz = data.size();
		if (sz != 0) {
			data[sz - 1].endTime = tm;
		}

		data.push_back(item);
	}

}


void Lyric::parseHeader(const std::string &tag)
{
	int pos = tag.find_first_of(':');
	if (pos == std::string::npos) {
		return;
	}

	const std::string &header = tag.substr(0, pos);
	const std::string &content = tag.substr(pos+1);

	if (header == "ar") {
		ar = std::move(content);
	} else if (header == "ti") {
		ti = std::move(content);
	}
}


LyricTime Lyric::parseTime(const std::string &tag)
{
	const char *buf = tag.c_str();
	int min;
	int sec;
	int msec;

	sscanf(buf, "%d:%d.%d", &min, &sec, &msec);

	LyricTime tm = 0;

	tm += min * 60 * 1000;
	tm += sec * 1000;
	tm += msec;

	return tm;
}


bool Lyric::parseFile(const std::string &file)
{
	std::ifstream in(file);

	if(!in.is_open()) {
		return false;
	}

	std::string buf;

	do {
		std::getline(in, buf);

		if (!buf.empty()) {
			parseLine(buf);
		}
	} while (in);

	in.close();
	return true;
}


int Lyric::binarySearchLyric(LyricTime tm) const
{
	int low = 0;
	int high = data.size() - 1;
	int mid;
	
	while (low <= high) {
		mid = (low + high) / 2;
		
		if (inRange(data[mid].startTime, data[mid].endTime, tm)) {
			return mid;
		}

		if (data[mid].startTime > tm) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}

	return -1;
}


bool Lyric::inRange(LyricTime start, LyricTime end, LyricTime tm) const
{
	if (end == 0) {
		return true;
	}
	
	return (tm > start && tm <= end);
}


const std::string& Lyric::getLyric(LyricTime tm, int *index) const
{
	int idx = binarySearchLyric(tm);
	
	if (index) {
		*index = idx;
	}

	return getLyricAt(idx);
}


const std::string& Lyric::getLyricAt(int index) const
{
	static std::string empty = "";
	
	if (index < 0 || index >= data.size()) {
		return empty;
	}
	
	return data[index].lyric;
}


const std::string& Lyric::getArtist() const
{
	return ar;
}


const std::string& Lyric::getTitle() const
{
	return ti;
}


bool Lyric::hasData() const
{
	return data.size() > 0;
}

