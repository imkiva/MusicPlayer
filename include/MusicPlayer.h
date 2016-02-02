#pragma once

#include <stdint.h>

#include <SLES/OpenSLES.h>

#include <mutex>
#include <string>
#include <functional>

namespace kiva {

enum PlayState
{
	NOT_READY,
	WAITING,
	PLAYING,
	PAUSED,
	STOPPED
};


class MusicPlayer
{
private:
	PlayState playState = NOT_READY;

	SLObjectItf slObject = NULL;
	SLEngineItf slEngine = NULL;
	SLObjectItf slOutputMix = NULL;
	
	SLEnvironmentalReverbItf outputMixEnvReverb = NULL;
	const SLEnvironmentalReverbSettings reverbSettings =	SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

	SLObjectItf uriPlayerObject = NULL;
	SLPlayItf uriPlayerPlay = NULL;
	SLSeekItf uriPlayerSeek = NULL;
	
	std::function<void()> onFinishCallback;
	std::mutex mutex;

private:
	bool initMusicPlayer();
	void initMusicEngine();
	
	bool loadMusic(const char *uriPath);
	void setPlaying(bool played);
	void destroyMusicPlayer();

	void onFinishInternal();

public:
	MusicPlayer();
	~MusicPlayer();
	
	bool setSource(const std::string &path);
	void setCallback(const std::function<void()> &cb);
	PlayState getState();
	SLmillisecond getDuration();
	SLmillisecond getPosition();
	
	void play();
	void pause();
	void stop();
	void seek(SLmillisecond pos);
};

}
