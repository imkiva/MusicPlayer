#pragma once

#include <stdint.h>

#include <SLES/OpenSLES.h>

#include <mutex>
#include <string>
#include <functional>

#include "EventEmiter.h"

namespace kiva {

typedef uint32_t Millisecond;

enum PlayState
{
	NOT_READY,
	WAITING,
	PLAYING,
	PAUSED,
	STOPPED
};


/**
 * EventEmiter:
 *  finish : current sone finish playing
 */
class Player : public EventEmiter<std::string>
{
public:
	virtual void play() = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
	virtual void seek(Millisecond pos) = 0;
	
	virtual PlayState getState() = 0;
	virtual Millisecond getDuration() = 0;
	virtual Millisecond getPosition() = 0;
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
	void destroyMusicPlayer();
	void onFinishInternal();
	
	void setPlaying(bool played);

public:
	MusicPlayer();
	~MusicPlayer();
	
	bool setSource(const std::string &path);
	void setCallback(const std::function<void()> &cb);
	PlayState getState();
	Millisecond getDuration();
	Millisecond getPosition();
	
	void play();
	void pause();
	void stop();
	void seek(Millisecond pos);
};

}
