#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

extern "C" {
	#include <libavformat/avformat.h>
}

#include "Player.h"


namespace kiva {

class SDL2MusicPlayer : public Player
{
	friend void hookMusicFinish();

private:
	volatile PlayState playState = NOT_READY;
	Mix_Music *music = NULL;
	AVFormatContext *formatContext = NULL;

	std::thread clockThread;
	volatile Millisecond currentPostion;

	std::mutex mutex;

	bool initSdl2Mixer();
	void destroySdl2Mixer();
	bool initMusicPlayer();
	void destroyMusicPlayer();

	void onFinishInternal();
	void setPlaying(bool playing);

	void initFormatContext(const std::string &path);

public:
	static SDL2MusicPlayer *current;

public:
	SDL2MusicPlayer();
	~SDL2MusicPlayer();

	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual void seek(Millisecond pos);
	virtual bool setSource(const std::string &path);
	
	virtual PlayState getState();
	virtual Millisecond getDuration();
	virtual Millisecond getPosition();
};

}

