#pragma once

#include <thread>


#define THREAD_SLEEP(v) \
	((void) std::this_thread::sleep_for(std::chrono::milliseconds(v)));



