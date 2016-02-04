#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Config.h"
#include "LocalCache.h"
#include "MainUI.h"

#define ALENGTH(x) (sizeof(x)/sizeof(x[0]))

using namespace std;
using namespace kiva;

static char bar[] = {
	'|', '/', '-', '\\'
};

static int barIndex = 0;


int main(int argc, char **argv) {
	opterr = 0;
	int result;
	
	bool update = false;
	
	while ((result = getopt(argc, argv, "u")) != -1) {
		switch(result) {
		case 'u': /* update cache */
			update = true;
			break;
		}
	}
	
	Config *cfg = Config::get();
	const std::string &data = cfg->getDataPath();
	const std::string &cacheFile = cfg->getCacheFile();
	
	if (update) {
		unlink(cacheFile.c_str());
	}
	
	MainUI ui;
	LocalCache cache(cacheFile);

	auto err = [&](void *p) {
		printf("缓存不存在或已过期, 重新扫描\n");
		
		MusicScanner sr(data);
		
		sr.on("finish", [&](void *p) {
			printf("扫描完毕\n");
			cache.writePlayListCache(sr.getResult());
			ui.setData(sr.getResult());
		});
		
		sr.on("progress-bar", [&](void *p) {
			*((int*) p) = 100; // interval
			
			printf("%c\r", bar[barIndex++]);
			fflush(stdout);
			
			if (barIndex >= ALENGTH(bar)) {
				barIndex = 0;
			}
		});
		
		sr.start();
	};
	
	auto ok = [&](void *p) {
		printf("缓存读取成功\n");
		vector<MusicEntry> *pv = (vector<MusicEntry>*) p;
		
		ui.setData(*pv);
	};
	
	cache.on("error", err);
	cache.on("expired", err);
	cache.on("ok", ok);
	
	printf("检查缓存\n");
	cache.readPlayListCache();
	
	return ui.exec();
}
