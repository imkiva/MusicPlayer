#pragma once

#include "koll/Koll.h"

namespace koll {

class Keyboard {
private:
	std::map<std::pair<char, char>, std::function<void(const int&)>> callback;
	std::thread thr;
	std::mutex mutex;
	
	volatile bool enabled;
	volatile char handled;
	
	static Keyboard sDefault;

public:
	static char ANY;
	static char OTHER;
	static char UP;
	static char DOWN;
	static char LEFT;
	static char RIGHT;
	
public:
	Keyboard();
	
	~Keyboard();
	
	void on(const char &ch, const std::function<void(const int&)> &cb);
	void on(const char &from, const char &to, const std::function<void(const int&)> &cb);
	
	void start();
	void stop();
	
private:
	void onKeyDown(const char &key);
	bool accept(const char &key, const char &from, const char &to);
	
public:
	static Keyboard* getDefault();
	
	static int getch(bool echo = false);
	static int keyboardHit();
};

}
