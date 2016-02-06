#include "OpenSLESMusicPlayer.h"


using namespace kiva;


OpenSLESMusicPlayer::OpenSLESMusicPlayer()
{
	initMusicPlayer();
}


OpenSLESMusicPlayer::~OpenSLESMusicPlayer()
{
	stop();
	destroyMusicPlayer();
}


Millisecond OpenSLESMusicPlayer::getDuration()
{
	if (!uriPlayerPlay) {
		return 0;
	}
	
	SLmillisecond duration;
	(*uriPlayerPlay)->GetDuration(uriPlayerPlay, &duration);
	
	return duration;
}


void OpenSLESMusicPlayer::seek(Millisecond pos)
{
	std::lock_guard<std::mutex> lock(mutex);
	
	if (!uriPlayerPlay) {
		return;
	}
	
	SLmillisecond dur = getDuration();
	if (pos > dur) {
		pos = dur;
	}
	
	(*uriPlayerSeek)->SetPosition(uriPlayerSeek, pos, SL_SEEKMODE_FAST);
}


Millisecond OpenSLESMusicPlayer::getPosition()
{
	if (!uriPlayerPlay) {
		return 0;
	}
	
	SLmillisecond pos;
	(*uriPlayerPlay)->GetPosition(uriPlayerPlay, &pos);
	
	return pos;
}


bool OpenSLESMusicPlayer::setSource(const std::string &path)
{
	std::lock_guard<std::mutex> lock(mutex);
	
	if (playState != WAITING) {
		stop();
		destroyMusicPlayer();
		initMusicPlayer();
	}
	
	const char *uriPath = path.c_str();
	
	SLDataLocator_URI loc_uri = { SL_DATALOCATOR_URI, (SLchar *) uriPath};
	SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
	SLDataSource audioSrc = { &loc_uri, &format_mime };
	SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, slOutputMix};
	SLDataSink audioSnk = { &loc_outmix, NULL };

	static const SLInterfaceID ids[] = {
		SL_IID_SEEK,
	};
	
	static const SLboolean req[] = {
		SL_BOOLEAN_TRUE
	};
	
	SLresult res;
	
	res = (*slEngine)->CreateAudioPlayer(slEngine, &uriPlayerObject,&audioSrc, &audioSnk, 1, ids, req);
	if (res != SL_RESULT_SUCCESS) {
		if (uriPlayerObject) {
			(*uriPlayerObject)->Destroy(uriPlayerObject);
		}
		return false;
	}

	res = (*uriPlayerObject)->Realize(uriPlayerObject, SL_BOOLEAN_FALSE);
	if (res != SL_RESULT_SUCCESS) {
		(*uriPlayerObject)->Destroy(uriPlayerObject);
		uriPlayerObject = NULL;
		return false;
	}

	(*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PLAY,&uriPlayerPlay);
	
	(*uriPlayerPlay)->RegisterCallback(uriPlayerPlay, [](SLPlayItf self, void *pthis, SLuint32 ev) {
		OpenSLESMusicPlayer *thiz = (OpenSLESMusicPlayer*) pthis;
		
		if (ev & SL_PLAYEVENT_HEADATEND) {
			thiz->onFinishInternal();
		}
	}, this);

	(*uriPlayerPlay)->SetCallbackEventsMask(uriPlayerPlay,SL_PLAYEVENT_HEADATEND);
	
	(*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_SEEK,&uriPlayerSeek);

	(*uriPlayerSeek)->SetLoop(uriPlayerSeek, SL_BOOLEAN_FALSE, 0,SL_TIME_UNKNOWN);
	return true;
}


PlayState OpenSLESMusicPlayer::getState()
{
	return playState;
}


void OpenSLESMusicPlayer::play()
{
	switch (playState) {
	case STOPPED:
		seek(0);
		/* no break */
	case WAITING:
		setPlaying(true);
		playState = PLAYING;
		break;
	
	case PAUSED:
		pause();
		break;
	}
}


void OpenSLESMusicPlayer::pause()
{
	switch (playState) {
	case PLAYING:
		setPlaying(false);
		playState = PAUSED;
		break;
	case PAUSED:
		setPlaying(true);
		playState = PLAYING;
		break;
	}
}


void OpenSLESMusicPlayer::stop()
{
	setPlaying(false);
	playState = STOPPED;
}


bool OpenSLESMusicPlayer::initMusicPlayer()
{
	slCreateEngine(&slObject, 0, NULL, 0, NULL, NULL);
	
	(*slObject)->Realize(slObject, SL_BOOLEAN_FALSE);
	
	(*slObject)->GetInterface(slObject, SL_IID_ENGINE,&slEngine);
	
	static const SLInterfaceID ids[] = {
		SL_IID_ENVIRONMENTALREVERB
	};
	
	static const SLboolean req[] = {
		SL_BOOLEAN_FALSE,
	};
	
	(*slEngine)->CreateOutputMix(slEngine, &slOutputMix,1, ids, req);
	(*slOutputMix)->Realize(slOutputMix, SL_BOOLEAN_FALSE);
	
	playState = WAITING;
	
	return true;
}


void OpenSLESMusicPlayer::setPlaying(bool ing)
{
	if (uriPlayerPlay) {
		(*uriPlayerPlay)->SetPlayState(uriPlayerPlay, (ing ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED));
	}
}


void OpenSLESMusicPlayer::onFinishInternal()
{
	playState = WAITING;
	
	this->emit("finish");
}


void OpenSLESMusicPlayer::destroyMusicPlayer()
{
	if (uriPlayerObject != NULL) {
		(*uriPlayerObject)->Destroy(uriPlayerObject);
		uriPlayerObject = NULL;
		uriPlayerPlay = NULL;
	}

	if (slOutputMix != NULL) {
		(*slOutputMix)->Destroy(slOutputMix);
		slOutputMix = NULL;
		outputMixEnvReverb = NULL;
	}

	if (slObject != NULL) {
		(*slObject)->Destroy(slObject);
		slObject = NULL;
	}
	
	playState = NOT_READY;
}

