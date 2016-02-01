#include "koll/Screen.h"
#include "koll/Text.h"

using namespace koll;

Screen::Screen()
{
	signal(SIGWINCH, Screen::winchHandler);
	update();
}

Screen::~Screen()
{
}

void Screen::update()
{
	ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
}

void Screen::onWinchSignal()
{
	update();
	std::for_each(std::begin(winchCallback), std::end(winchCallback), [&](const std::function<void()> &cb) {
		cb();
	});
}

int Screen::getWidth()
{
	return win.ws_col;
}

int Screen::getHeight()
{
	return win.ws_row;
}

void Screen::onWinch(const std::function<void()> &cb)
{
	winchCallback.push_back(cb);
}

Screen* Screen::get()
{
	return &sInstance;
}

void Screen::gotoxy(int x, int y)
{
	printf("\x1B[%d;%df", y, x);
}

void Screen::clear()
{
	printf("\x1B[2J\x1B[0;0f");
}

void Screen::hideCursor()
{
	printf("\033[?25l");
}

void Screen::showCursor()
{
	printf("\033[?25h");
}

void Screen::mprintf(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char buffer[512];
	vsnprintf(buffer, 512, fmt, arg);
	Screen::mprint(buffer);

	va_end(arg);
}

void Screen::mprint(const std::string &str)
{
	int width = Screen::get()->getWidth();
	int swidth = Text::getStringWidth(str);

	std::string pstr;
	std::string rstr;
	bool print2 = false;

	if(swidth > width) {
		int pos = Text::getPartString(str, width);
		pstr = str.substr(0, pos);
		rstr = str.substr(pos);
		print2 = true;
	} else {
		pstr = str;
	}

	int pwidth = Text::getStringWidth(pstr);
	int spaces = (width - pwidth) >> 1;

	/*for (int i=0; i<spaces; ++i) {
		putchar(' ');
	}*/
	
	printf("%*c%s", spaces, ' ', pstr.c_str());

	if (print2) {
		putchar('\n');
		Screen::mprint(rstr);
	}
}


void Screen::terminal_lnbuf(int yn)
{
	struct termios oldt, newt;
	tcgetattr(0, &oldt);
	newt = oldt;
	if (!yn)
		newt.c_lflag &= ~ICANON;
	else newt.c_lflag |= ICANON;
	tcsetattr(0, TCSANOW, &newt);
}

void Screen::terminal_echo(int yn)
{
	struct termios oldt, newt;
	tcgetattr(0, &oldt);
	newt = oldt;
	if (!yn)
		newt.c_lflag &= ~ECHO;
	else newt.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &newt);
}

void Screen::winchHandler(int winch)
{
	Screen *scr = Screen::get();
	scr->onWinchSignal();
}

Screen Screen::sInstance;
