#include "koll/Keyboard.h"
#include "koll/Screen.h"

using namespace koll;

Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
	stop();
}

void Keyboard::on(const char &ch, const std::function<void(const int&)> &cb)
{
	on(ch, ch, cb);
}

void Keyboard::on(const char &from, const char &to, const std::function<void(const int&)> &cb)
{
	callback[std::pair<char, char>(from, to)] = cb;
}

void Keyboard::start()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (enabled) {
		return;
	}

	Screen::hideCursor();
	enabled = true;

	thr = std::thread([&] {
		int ch;
		while(enabled)
		{
			if (Keyboard::keyboardHit()) {
				ch = Keyboard::getch();
				
				if (ch == 27) { /* cursor key */
					Keyboard::getch(); /* skip '[' */
					ch = Keyboard::getch();
					
					ch = -ch;
				}
				
				this->onKeyDown(ch);
			}
		}
	});
}

void Keyboard::stop()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (!enabled) {
		return;
	}

	Screen::showCursor();
	enabled = false;
	thr.join();
}

void Keyboard::onKeyDown(const char &key)
{
	handled = 0;
	for (auto &i : callback) {
		if (accept(key, i.first.first, i.first.second)) {
			if ((handled & key) == 0) {
				handled |= key;
				i.second(key);
			}
		}
	}
}

bool Keyboard::accept(const char &key, const char &from, const char &to)
{
	return (from == Keyboard::ANY)
		   || (to == Keyboard::ANY)
		   || (key >= from && key <= to)
		   || (from == Keyboard::OTHER)
		   || (to == Keyboard::OTHER);
}

Keyboard* Keyboard::getDefault()
{
	return &sDefault;
}

int Keyboard::getch(bool echo)
{
	register int ch;
	Screen::terminal_lnbuf(0);
	if (!echo)
		Screen::terminal_echo(0);

	ch = getchar();

	Screen::terminal_lnbuf(1);
	if (!echo)
		Screen::terminal_echo(1);
	return ch;
}

int Keyboard::keyboardHit()
{
	register int ret;
	fd_set fds;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	Screen::terminal_lnbuf(0);
	Screen::terminal_echo(0);

	FD_ZERO(&fds);
	FD_SET(0, &fds);
	select(1, &fds, 0, 0, &tv);
	ret = FD_ISSET(0, &fds);

	Screen::terminal_lnbuf(1);
	Screen::terminal_echo(1);
	return ret;
}

Keyboard Keyboard::sDefault;
char Keyboard::ANY = -1;
char Keyboard::OTHER = -2;

char Keyboard::UP = -'A';
char Keyboard::DOWN = -'B';
char Keyboard::LEFT = -'D';
char Keyboard::RIGHT = -'C';
