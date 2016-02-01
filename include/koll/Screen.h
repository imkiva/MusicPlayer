#pragma once

#include "koll/Koll.h"

namespace koll {

class Screen {
private:
	static Screen sInstance;
	
	std::vector<std::function<void()>> winchCallback;
	struct winsize win;
	

private:
	Screen();
	
	~Screen();
	
	void update();
	
	void onWinchSignal();

public:
	int getWidth();
	
	int getHeight();
	
	void onWinch(const std::function<void()> &cb);
	
public:
	static Screen* get();
	
	static void gotoxy(int x, int y);
	
	static void clear();
	
	static void hideCursor();
	
	static void showCursor();
	
	static void mprintf(const char *fmt, ...);
	
	static void mprint(const std::string &str);

	
	static void terminal_lnbuf(int yn);
	
	static void terminal_echo(int yn);

private:
	static void winchHandler(int winch);
};

}
