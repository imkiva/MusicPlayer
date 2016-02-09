#include <stdlib.h>
#include <unistd.h>

#include "Config.h"

using namespace kiva;

static std::string getDataPathFromEnv()
{
	std::string data;
	
	const char *p = getenv(DATA_ENV);
	if (p) {
		data.assign(p);
	} else {
		data.assign(DATA_DEFAULT_PATH);
	}
	
	return std::move(data);
}

Config* Config::get()
{
	static Config config;
	return &config;
}


Config::Config()
{
	dataPath = std::move(getDataPathFromEnv());
	cacheFile = std::move(dataPath + CACHEFILE_SUFFIX);
	configFile = std::move(dataPath + CFGFILE_SUFFIX);
	
	ini.load(configFile);
	
	Section &s = ini[CFG_SECTION];
	if (!s.contains(KEY_SHOW_HELP)) {
		s[KEY_SHOW_HELP] = true;
	}
	
	if (!s.contains(KEY_SHOW_PLAYLIST)) {
		s[KEY_SHOW_PLAYLIST] = true;
	}
	
	if (!s.contains(KEY_PLAYMODE)) {
		s[KEY_PLAYMODE] = (int) LOOP_ALL;
	}
	
	if (!s.contains(KEY_CURRENTITEM)) {
		s[KEY_CURRENTITEM] = 0;
	}
	
	if (!s.contains(KEY_CURRENTPAGE)) {
		s[KEY_CURRENTPAGE] = 0;
	}
	
	if (!s.contains(KEY_CURRENTPOS)) {
		s[KEY_CURRENTPOS] = 0;
	}
}


Config::~Config()
{
	ini.save();
}


const std::string& Config::getDataPath()
{
	return dataPath;
}


const std::string& Config::getCacheFile()
{
	return cacheFile;
}


const std::string& Config::getConfigFile()
{
	return configFile;
}
	

bool Config::showHelp()
{
	return ini[CFG_SECTION][KEY_SHOW_HELP].asBool();
}


bool Config::showPlayList()
{
	return ini[CFG_SECTION][KEY_SHOW_PLAYLIST].asBool();
}


PlayMode Config::getPlayMode()
{
	return (PlayMode) ini[CFG_SECTION][KEY_PLAYMODE].asInt();
}


int Config::getCurrentItem()
{
	return ini[CFG_SECTION][KEY_CURRENTITEM].asInt();
}


Millisecond Config::getCurrentPosition()
{
	int i = ini[CFG_SECTION][KEY_CURRENTPOS].asInt();
	
	return i < 0 ? 0 : (Millisecond) i;
}


int Config::getCurrentPage()
{
	return ini[CFG_SECTION][KEY_CURRENTPAGE].asInt();
}


void Config::setShowHelp(bool b)
{
	ini[CFG_SECTION][KEY_SHOW_HELP] = b;
}


void Config::setShowPlayList(bool b)
{
	ini[CFG_SECTION][KEY_SHOW_PLAYLIST] = b;
}


void Config::setPlayMode(PlayMode mode)
{
	ini[CFG_SECTION][KEY_PLAYMODE] = (int) mode;
}


void Config::setCurrentItem(int pos)
{
	ini[CFG_SECTION][KEY_CURRENTITEM] = pos;
}


void Config::setCurrentPosition(Millisecond pos)
{
	ini[CFG_SECTION][KEY_CURRENTPOS] = (int) pos;
}


void Config::setCurrentPage(int page)
{
	ini[CFG_SECTION][KEY_CURRENTPAGE] = page;
}
