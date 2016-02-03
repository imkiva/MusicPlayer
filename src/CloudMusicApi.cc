#include "json/json.h"
#include "LyricDownloader.h"

using namespace kiva;

static void setupHttpHeader(Http &http)
{
	http.addHeader("Referer", "http://music.163.com");
	http.addHeader("Cookie", "appver=2.0.2;");
	http.addHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.106 Safari/537.36");
	http.addHeader("Accept", "application/json");
	http.addHeader("Connection", "close");
}


CloudMusicApi::CloudMusicApi()
{

}


CloudMusicApi::~CloudMusicApi()
{

}


int CloudMusicApi::getMusicId(const std::string &name)
{
	try {
		Http http(host, idParam + Http::urlEncode(name));
		http.setMethod(POST);

		setupHttpHeader(http);
		http.request();
		
		Json::Reader r;
		Json::Value v;
		
		if (!r.parse(http.getResponse(), v)) {
			return -1;
		}
		
		if (v["result"]["songCount"].asInt() == 0) {
			return -1;
		}
		
		return v["result"]["songs"][0]["id"].asInt();
		
	} catch (...) {
		return -1;
	}

	return -1;
}


bool CloudMusicApi::downloadLyric(const std::string &musicName, std::string &lyric)
{
	int id = getMusicId(musicName);

	if (id == -1) {
		return false;
	}

	std::stringstream ss;
	ss << id;

	try {
		Http http(host, lyricParam + ss.str());
		http.setMethod(GET);

		setupHttpHeader(http);
		http.request();
		
		Json::Reader r;
		Json::Value v;
		
		if (!r.parse(http.getResponse(), v)) {
			return false;
		}
		
		lyric = std::move(v["lrc"]["lyric"].asString());
		//lyric = std::move(http.getResponse());
		return true;
	} catch (...) {
		return false;
	}

	return false;
}

/*
int main(int argc, char const *argv[])
{
	MusicApi *a = new CloudMusicApi();
	
	LyricDownloader d(a);
	volatile bool q = false;

	d.on("finish", [&](void *p) {
		DownloadedLyric *s = (DownloadedLyric*) p;
		printf("%s\n%s\n", s->name->c_str(), s->lyric->c_str());
		
		q = true;
	});
	
	d.on("error", [&](void *p) {
		q = true;
	});
	
	d.downloadLyric("走在冷风中");
	d.downloadLyric("烟花易冷");

	while (!q) { }
	return 0;
}
*/

