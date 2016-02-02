#pragma once

#include <dirent.h>

#include <thread>
#include <mutex>
#include <vector>
#include <functional>

#include "MusicEntry.h"
#include "EventEmiter.h"

namespace kiva {

class MusicScanner : public EventEmiter<std::string>
{
private:
	std::thread scanThread;
	std::thread progressBarThread;
	std::vector<MusicEntry> result;
	
	std::string root;
	
	volatile bool finish;

	void doScanDir(const std::string &path);
	
public:
	MusicScanner(const std::string &root);
	virtual ~MusicScanner();
	
	void start();
	void clearResult();
	const std::vector<MusicEntry>& getResult();
};

}
