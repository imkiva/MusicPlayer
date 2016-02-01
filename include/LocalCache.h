#pragma once

#include <stdio.h>
#include <time.h>

#include <thread>
#include <vector>
#include <string>
#include <functional>

#include "EventEmiter.h"
#include "MusicEntry.h"

/* 缓存的有效时间 */
/* 1天, 单位: 秒 */
#define CACHE_EFFECT (60 * 60 * 24)

namespace kiva {

class LocalCache : public EventEmiter<std::string> {
private:
	std::string file;
	std::thread thr;
	
	bool isExpired(time_t &created, time_t now, time_t effect);
	time_t readCreatedTime();

public:
	LocalCache(const std::string &file);
	~LocalCache();
	
	void readPlayListCache();
	void writePlayListCache(const std::vector<MusicEntry> &vec);
};

}
