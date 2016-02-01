#include <stdio.h>
#include <unistd.h>

#include "LocalCache.h"
#include "PlayerUI.h"

using namespace std;
using namespace kiva;

int main(int argc, char **argv) {
	argc--;
	argv++;
	
	std::string sdcard;
	
	if (argv[0]) {
		sdcard.assign(argv[0]);
	} else {
		const char *p = getenv("EXTERNAL_STORAGE");
		if (p) {
			sdcard.assign(p);
		} else {
			sdcard.assign("/sdcard");
		}
	}
	
	std::string cacheFile = sdcard + "/Android/.player_cache";
	
	PlayerUI ui;
	LocalCache cache(cacheFile);

	auto err = [&](void *p) {
		printf("缓存不存在或已过期, 重新扫描\n");
		
		MusicScanner sr(sdcard, [&]() {
			printf("扫描完毕\n");
			cache.writePlayListCache(sr.getResult());
			ui.setData(sr.getResult());
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
