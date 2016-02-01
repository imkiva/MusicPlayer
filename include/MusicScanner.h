#pragma once

#include <dirent.h>

#include <thread>
#include <mutex>
#include <vector>
#include <functional>

#include "MusicEntry.h"

namespace kiva {

class MusicScanner {
private:
	std::thread scanThread;
	std::vector<MusicEntry> result;
	
	std::string root;
	std::function<void()> onFinishCallback;

	void doScanDir(const std::string &path);
	
public:
	MusicScanner(const std::string &root, const std::function<void()> &cb);
	~MusicScanner();
	
	void start();
	void clearResult();
	const std::vector<MusicEntry>& getResult();
};

}
