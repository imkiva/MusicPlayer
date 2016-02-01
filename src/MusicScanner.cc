#include <sys/types.h>
#include <string.h>

#include <regex>

#include "MusicScanner.h"

using namespace kiva;

static bool accept(const std::string &name)
{
	static std::regex reg(".*(\\.mp3|\\.ogg|\\.wav)", std::regex_constants::icase);
	
	return std::regex_match(name, reg);
}


MusicScanner::MusicScanner(const std::string &root, const std::function<void()> &cb)
	:root(root), onFinishCallback(cb)
{
	
}


MusicScanner::~MusicScanner()
{
	if (scanThread.joinable()) {
		scanThread.join();
	}
}


void MusicScanner::start()
{
	if (scanThread.joinable()) {
		scanThread.join();
	}
	
	scanThread = std::thread([&]() {
		doScanDir(root);
		
		if (onFinishCallback) {
			onFinishCallback();
		}
	});
}


void MusicScanner::clearResult()
{
	result.clear();
}


void MusicScanner::doScanDir(const std::string &path)
{
	DIR *dir;
	struct dirent *e;
	
	dir = opendir(path.c_str());
	if (!dir) {
		return;
	}
	
	while ((e = readdir(dir)) != NULL) {
		if (e->d_type == DT_REG) {
			if (accept(e->d_name)) {
				MusicEntry ent(path + '/' + e->d_name);
				
				result.push_back(ent);
			}
		} else if (e->d_type == DT_DIR) {
			if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) {
				continue;
			}
			
			doScanDir(path + '/' + e->d_name);
		}
	}
		
	closedir(dir);
}


const std::vector<MusicEntry>& MusicScanner::getResult()
{
	return result;
}

