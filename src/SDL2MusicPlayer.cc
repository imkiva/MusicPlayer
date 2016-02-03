#include "SDL2MusicPlayer.h"

using namespace kiva;


SDL2MusicPlayer::SDL2MusicPlayer()
{
	current = this;

	initMusicPlayer();
}


SDL2MusicPlayer::~SDL2MusicPlayer()
{
	stop();
	destroyMusicPlayer();
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

	music = Mix_LoadMUS(path.c_str());
	if (!music) {
		return false;
	}

	Mix_HookMusicFinished([]() {
		SDL2MusicPlayer::current->onFinishInternal();
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
}


Millisecond SDL2MusicPlayer::getPosition()
{
	return 0;
}


Millisecond SDL2MusicPlayer::getDuration()
{
	return 0;
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

