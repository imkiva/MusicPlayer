#pragma once

#include <stdint.h>

#include <mutex>
#include <string>
#include <functional>

#include "EventEmiter.h"

namespace kiva {

typedef uint32_t Millisecond;

enum PlayMode
{
	LOOP_ALL = 0,
	LOOP_ONE = 1,
	RANDOM   = 2,
};

enum PlayState
{
	NOT_READY = 0,
	WAITING   = 1,
	PLAYING   = 2,
	PAUSED    = 3,
	STOPPED   = 4,
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
	virtual bool setSource(const std::string &path) = 0;
	
	virtual PlayState getState() = 0;
	virtual Millisecond getDuration() = 0;
	virtual Millisecond getPosition() = 0;
};

}
