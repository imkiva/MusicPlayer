#pragma once

#include "Player.h"
#include "IniFile.h"

#ifdef ANDROID
#	define DATA_ENV "EXTERNAL_STORAGE"
#	define DATA_DEFAULT_PATH "/sdcard"
#	define CACHEFILE_SUFFIX "/Android/.player_cache"
#	define CFGFILE_SUFFIX "/Android/.player_cfg.ini"
#else
#	define DATA_ENV "HOME"
#	define DATA_DEFAULT_PATH "/tmp"
#	define CACHEFILE_SUFFIX "/.player_cache"
#	define CFGFILE_SUFFIX "/.player_cfg.ini"
#endif

#define CFG_SECTION "player"
#define KEY_SHOW_HELP "show_help"
#define KEY_SHOW_PLAYLIST "show_playlist"
#define KEY_PLAYMODE "playmode"
#define KEY_CURRENTITEM "current_item"
#define KEY_CURRENTPAGE "current_page"
#define KEY_CURRENTPOS  "current_pos"

/* 默认显示帮助 */
#define DEFAULT_SHOW_HELP true

/* 默认显示播放列表 */
#define DEFAULT_SHOW_PLAY_LIST true

/* 默认播放模式: 列表循环 */
#define DEFAULT_PLAY_MODE LOOP_ALL


namespace kiva {

class Config
{
private:
	IniFile ini;
	std::string dataPath;
	std::string cacheFile;
	std::string configFile;
	
	Config();
	~Config();
	
public:
	static Config* get();
	
	const std::string& getDataPath();
	const std::string& getCacheFile();
	const std::string& getConfigFile();
	
	bool showHelp();
	bool showPlayList();
	PlayMode getPlayMode();
	int getCurrentItem();
	int getCurrentPage();
	Millisecond getCurrentPosition();
	
	void setShowHelp(bool b);
	void setShowPlayList(bool b);
	void setPlayMode(PlayMode mode);
	void setCurrentItem(int pos);
	void setCurrentPage(int page);
	void setCurrentPosition(Millisecond pos);
};

}

