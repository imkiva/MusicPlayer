#include "LyricDownloader.h"

using namespace kiva;

LyricDownloader::LyricDownloader(MusicApi *api)
{
	setApi(api);
}


LyricDownloader::~LyricDownloader()
{
	if (downloadThread.joinable()) {
		downloadThread.join();
	}

	delete this->api;
}


void LyricDownloader::setApi(MusicApi *api)
{
	this->api = api;
}


void LyricDownloader::downloadLyric(MusicEntry *entry)
{
	if (!api) {
		return;
	}
	
	if (downloadThread.joinable()) {
		downloadThread.join();
	}

	downloadThread = std::thread([=]() {
		std::string lyric;
		
		DownloadedLyric ly;
		ly.ent = entry;
		ly.lyric = &lyric;
		
		if (api->downloadLyric(entry->getName(), lyric)) {
			FILE *fp = fopen(entry->getLyricPath().c_str(), "w");
			if (fp) {
				fprintf(fp, "%s\n", lyric.c_str());
				fclose(fp);
				
				entry->loadLyric(entry->getLyricPath());
			}
			
			this->emit("finish", (void*) &ly);
		} else {
			this->emit("error");
		}
	});
}

