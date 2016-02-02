#include "MusicPlayer.h"


using namespace kiva;


MusicPlayer::MusicPlayer()
{
	initMusicPlayer();
}


MusicPlayer::~MusicPlayer()
{
	stop();
	destroyMusicPlayer();
}


Millisecond MusicPlayer::getDuration()
{
	if (!uriPlayerPlay) {
		return 0;
	}
	
	SLmillisecond duration;
	(*uriPlayerPlay)->GetDuration(uriPlayerPlay, &duration);
	
	return duration;
}


void MusicPlayer::seek(Millisecond pos)
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


Millisecond MusicPlayer::getPosition()
{
	if (!uriPlayerPlay) {
		return 0;
	}
	
	SLmillisecond pos;
	(*uriPlayerPlay)->GetPosition(uriPlayerPlay, &pos);
	
	return pos;
}


bool MusicPlayer::setSource(const std::string &path)
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
	
	(*slEngine)->CreateAudioPlayer(slEngine, &uriPlayerObject,&audioSrc, &audioSnk, 1, ids, req);

	SLresult res = (*uriPlayerObject)->Realize(uriPlayerObject, SL_BOOLEAN_FALSE);
	if (res != SL_RESULT_SUCCESS) {
		(*uriPlayerObject)->Destroy(uriPlayerObject);
		uriPlayerObject = NULL;
		return false;
	}

	(*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PLAY,&uriPlayerPlay);
	
	(*uriPlayerPlay)->RegisterCallback(uriPlayerPlay, [](SLPlayItf self, void *pthis, SLuint32 ev) {
		MusicPlayer *thiz = (MusicPlayer*) pthis;
		if (ev & SL_PLAYEVENT_HEADATEND) {
			thiz->onFinishInternal();
		}
	}, this);

	(*uriPlayerPlay)->SetCallbackEventsMask(uriPlayerPlay,SL_PLAYEVENT_HEADATEND);
	
	(*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_SEEK,&uriPlayerSeek);

	(*uriPlayerSeek)->SetLoop(uriPlayerSeek, SL_BOOLEAN_FALSE, 0,SL_TIME_UNKNOWN);
	return true;
}


PlayState MusicPlayer::getState()
{
	return playState;
}


void MusicPlayer::play()
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


void MusicPlayer::pause()
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


void MusicPlayer::stop()
{
	setPlaying(false);
	playState = STOPPED;
}


void MusicPlayer::setCallback(const std::function<void()> &cb)
{
	this->onFinishCallback = cb;
}


bool MusicPlayer::initMusicPlayer()
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


void MusicPlayer::setPlaying(bool ing)
{
	if (uriPlayerPlay) {
		(*uriPlayerPlay)->SetPlayState(uriPlayerPlay, (ing ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED));
	}
}


void MusicPlayer::onFinishInternal()
{
	playState = WAITING;
	
	if (onFinishCallback) {
		onFinishCallback();
	}
}


void MusicPlayer::destroyMusicPlayer()
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

