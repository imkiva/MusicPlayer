#pragma once

#include <SLES/OpenSLES.h>

#include "Player.h"


namespace kiva {

class OpenSLESMusicPlayer : public Player
{
private:
	PlayState playState = NOT_READY;

	SLObjectItf slObject = NULL;
	SLEngineItf slEngine = NULL;
	SLObjectItf slOutputMix = NULL;
	
	SLEnvironmentalReverbItf outputMixEnvReverb = NULL;
	const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

	SLObjectItf uriPlayerObject = NULL;
	SLPlayItf uriPlayerPlay = NULL;
	SLSeekItf uriPlayerSeek = NULL;
	
	std::mutex mutex;

private:
	bool initMusicPlayer();
	void destroyMusicPlayer();
	void onFinishInternal();
	
	void setPlaying(bool played);

public:
	OpenSLESMusicPlayer();
	~OpenSLESMusicPlayer();
	
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

