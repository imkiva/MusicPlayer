#pragma once


extern "C" {
	#include <unistd.h>
	#include <termios.h>
	#include <sys/select.h>
	#include <sys/ioctl.h>
	#include <signal.h>
	
	/* Screen */
	#include <stdarg.h>
}

#include <functional>
#include <algorithm>

/* Keyboard */
#include <map>
#include <thread>
#include <mutex>

/* Screen */
#include <vector>


