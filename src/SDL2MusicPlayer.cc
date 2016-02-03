#include "SDL2MusicPlayer.h"
#include "Thread.h"

using namespace kiva;


SDL2MusicPlayer::SDL2MusicPlayer()
{
	current = this;
	av_register_all();
	initMusicPlayer();
}


SDL2MusicPlayer::~SDL2MusicPlayer()
{
	stop();
	destroyMusicPlayer();

	if (clockThread.joinable()) {
		clockThread.join();
	}
}


bool SDL2MusicPlayer::initSdl2Mixer()
{
	static int flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG;

	if (SDL_Init(SDL_INIT_AUDIO) == -1) {
		return false;
	}

	int ret = Mix_Init(flags);

	if (ret & flags != flags) {
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		return false;
	}
	
	return true;
}


void SDL2MusicPlayer::destroySdl2Mixer()
{
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
}


bool SDL2MusicPlayer::initMusicPlayer()
{
	initSdl2Mixer();
	playState = WAITING;
	return true;
}


void SDL2MusicPlayer::destroyMusicPlayer()
{
	if (music) {
		Mix_FreeMusic(music);
		music = NULL;
	}

	destroySdl2Mixer();

	playState = NOT_READY;
}


void SDL2MusicPlayer::initFormatContext(const std::string &path)
{
	int ret;

	formatContext = avformat_alloc_context();
	if (!formatContext) {
		return;
	}

	ret = avformat_open_input(&formatContext, path.c_str(), NULL, NULL);
	if (ret < 0) {
		avformat_close_input(&formatContext);
		formatContext = NULL;
		return;
	}

	ret = avformat_find_stream_info(formatContext, NULL);
	if (ret < 0) {
		avformat_close_input(&formatContext);
		formatContext = NULL;
		return;
	}
}


PlayState SDL2MusicPlayer::getState()
{
	return playState;
}


void SDL2MusicPlayer::setPlaying(bool playing)
{
	if (!music) {
		return;
	}

	if (playing) {
		Mix_ResumeMusic();
	} else {
		Mix_PauseMusic();
	}
}


void SDL2MusicPlayer::stop()
{
	setPlaying(false);
	playState = STOPPED;
}


bool SDL2MusicPlayer::setSource(const std::string &path)
{
	if (playState != WAITING) {
		stop();
		destroyMusicPlayer();
		initMusicPlayer();
	}

	initFormatContext(path);

	music = Mix_LoadMUS(path.c_str());
	if (!music) {
		return false;
	}

	Mix_HookMusicFinished([]() {
		SDL2MusicPlayer::current->onFinishInternal();
	});

	if (clockThread.joinable()) {
		clockThread.join();
	}

	clockThread = std::thread([this]() {
		THREAD_SLEEP(1000);
		currentPostion = 1000;

		while (playState != NOT_READY && playState != WAITING) {
			if (playState == PLAYING) {
				currentPostion += 100;
				THREAD_SLEEP(100);
			}
		}

		currentPostion = 0;
	});

	Mix_PlayMusic(music, 0);
	playState = PLAYING;

	return true;
}


void SDL2MusicPlayer::pause()
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


void SDL2MusicPlayer::seek(Millisecond pos)
{
	if (!music) {
		return;
	}

	Mix_RewindMusic();
	Mix_SetMusicPosition((double) pos / 1000);
	currentPostion = pos;
}


Millisecond SDL2MusicPlayer::getPosition()
{
	return currentPostion;
}


Millisecond SDL2MusicPlayer::getDuration()
{
	if (!formatContext) {
		return 0;
	}

	if (formatContext->duration == AV_NOPTS_VALUE) {
		return 0;
	}

	int64_t duration = formatContext->duration;
	duration = duration + (duration <= INT64_MAX - 5000 ? 5000 : 0);
	duration /= 1000;

	return (Millisecond) duration;
}


void SDL2MusicPlayer::play()
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


void SDL2MusicPlayer::onFinishInternal()
{
	playState = WAITING;
	this->emit("finish");
}


SDL2MusicPlayer* SDL2MusicPlayer::current;

