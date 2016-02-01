#pragma once

#include "EventEmiter.h"
#include "MusicScanner.h"
#include "MusicEntry.h"
#include "MusicPlayer.h"

#include "koll/Keyboard.h"
#include "koll/Screen.h"

#define PAGE_ITEM 9

namespace kiva {

typedef enum {
	LOOP_ALL,
	LOOP_ONE,
	RANDOM
} PlayMode;

class PlayerUI : public EventEmiter<std::string> {
private:
	koll::Keyboard *keyboard;
	MusicPlayer player;
	
	std::vector<MusicEntry> data;
	volatile int current;
	volatile int page;
	volatile PlayMode playMode;
	
	SLmillisecond currDuration;
	SLmillisecond currPosition;
	const char *currName;
	const char *currArtist;
	PlayState currState;
	
	volatile bool finish;
	volatile bool needShowHelp;
	volatile bool needShowPlayList;

private:
	void play(int pos);
	void playNext();
	void playPrev();
	
	int getIndex(int page, int i);
	
	void showPlayList();
	void showHelp();
	void showLyric(const Lyric &ly);
	void showProgress();
	void printUI();
	
public:
	PlayerUI();
	virtual ~PlayerUI();
	
	void setData(const std::vector<MusicEntry> &data);
	int exec();
};

}

