#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include "LocalCache.h"

using namespace kiva;

LocalCache::LocalCache(const std::string &file)
	:file(file)
{

}


LocalCache::~LocalCache()
{
	if (thr.joinable()) {
		thr.join();
	}
}


time_t LocalCache::readCreatedTime()
{
	FILE *fp = fopen(file.c_str(), "r");
	if (!fp) {
		return 0;
	}
	
	char buf[80] = {0};
	fgets(buf, 80, fp);
	fclose(fp);
	
	time_t tm = 0;
	
	if (strstr(buf, "created: ")) {
		sscanf(buf, "created: %ld", &tm);
	}
	
	return tm;
}


bool LocalCache::isExpired(time_t &created, time_t now, time_t effect)
{
	created = readCreatedTime();
	return abs(now - created) > effect;
}


void LocalCache::readPlayListCache()
{
	thr = std::thread([&]() {
		time_t now = time(NULL);
		time_t effect = CACHE_EFFECT;
		time_t created;
		
		if (isExpired(created, now, effect)) {
			this->emit("expired");
			return;
		}
		
		FILE *fp = fopen(file.c_str(), "r");
		if (!fp) {
			this->emit("error");
			return;
		}
		
		std::vector<MusicEntry> v;
		char *buf = new char[PATH_MAX];
		fgets(buf, PATH_MAX, fp); /* skip created */
		
		while (!feof(fp)) {
			memset(buf, '\0', PATH_MAX);
			fgets(buf, PATH_MAX, fp);
			
			int l = strlen(buf) - 1;
			if (buf[l] == '\n') {
				buf[l] = '\0';
			}
			
			if (!strcmp("", buf)) {
				continue;
			}
			
			MusicEntry e(buf);
			v.push_back(e);
		}
		
		delete[] buf;
		fclose(fp);
		
		this->emit("ok", (void*) &v);
	});
}


void LocalCache::writePlayListCache(const std::vector<MusicEntry> &vec)
{
	FILE *fp = fopen(file.c_str(), "w");
	if (!fp) {
		return;
	}
	
	time_t tm;
	time(&tm);
	
	fprintf(fp, "created: %ld\n", tm);
	
	for (auto &e : vec) {
		fprintf(fp, "%s\n", e.getPath().c_str());
	}
	
	fclose(fp);
}

