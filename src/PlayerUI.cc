#include <iostream>

#include "Control.h"
#include "PlayerUI.h"

using namespace kiva;
using namespace koll;

static const char* getStateString(PlayState s)
{
	switch(s) {
	case WAITING:
		return "等待播放";
	case PLAYING:
		return "正在播放";
	case PAUSED:
		return "已暂停";
	case STOPPED:
		return "已停止";
	}
}


PlayerUI::PlayerUI()
{
	keyboard = Keyboard::getDefault();
	
	keyboard->on(CEXIT, [&](const int &key) {
		this->finish = true;
	});
	
	keyboard->on(CPAUSE, [&](const int &key) {
		this->emit("pause");
	});
	
	keyboard->on(CNEXT, [&](const int &key) {
		this->emit("next");
	});
	
	keyboard->on(CPREV, [&](const int &key) {
		this->emit("prev");
	});
	
	keyboard->on(CPAGENEXT, [&](const int &key) {
		this->emit("page-next");
	});
	
	keyboard->on(CPAGEPREV, [&](const int &key) {
		this->emit("page-prev");
	});
	
	keyboard->on(CHELP, [&](const int &key) {
		this->emit("toggle-help");
	});
	
	keyboard->on(CPLAYLIST, [&](const int &key) {
		this->emit("toggle-play-list");
	});
	
	keyboard->on('0', '9', [&](const int &key) {
		int k = key - '0';
		this->emit("numkey", (void*)k);
	});
	
	keyboard->on(CSEEKL, [&](const int &key) {
		this->emit("seek-left");
	});
	
	keyboard->on(CSEEKR, [&](const int &key) {
		this->emit("seek-right");
	});
}


PlayerUI::~PlayerUI()
{
}


void PlayerUI::play(int pos)
{
	if (pos < 0 || pos >= data.size()) {
		return;
	}
	
	current = pos;
	const MusicEntry &e = data[pos];
	
	player.setSource(e.getPath().c_str());
	player.play();
}


void PlayerUI::playNext()
{
	if (current == data.size()-1) {
		play(0);
	} else {
		play(current+1);
	}
}


void PlayerUI::playPrev()
{
	if (current == 0) {
		play(data.size()-1);
	} else {
		play(current-1);
	}
}


int PlayerUI::getIndex(int page, int i)
{
	return page * PAGE_ITEM + i;
}


void PlayerUI::showPlayList() {
	if (!needShowPlayList) {
		return;
	}
	
	printf("播放列表(%d/%d): \n", page+1, data.size()/PAGE_ITEM+1);
	
	bool check = true;
	auto it = data.begin() + getIndex(page, 0);
	
	for (int i=0; i<=PAGE_ITEM; it++, i++) {
		int idx = getIndex(page, i);
		if (idx >= data.size()) {
			break;
		}
		
		const std::string &n = (*it).getName();
		const std::string &a = (*it).getArtist();
		
		printf(" %d. ",i);
		if (check && idx == current) {
			check = false;
			printf("\e[1;33m");
		}
		
		printf("%s - %s\e[0m\n", n.c_str(), a.c_str());
	}
	
	printf("\n");
}


void PlayerUI::showHelp() {
	if (!needShowHelp) {
		return;
	}
	
	PlayState s = player.getState();
	
	printf("操作方式: \n");
	printf(" %s: %c  退出: %c", (s == PAUSED ? "继续" : "暂停"), CPAUSE, CEXIT);
	printf("  切换帮助: %c  %s: %c\n", CHELP, (needShowPlayList ? "隐藏列表" : "显示列表"), CPLAYLIST);
	
	printf(" 下一曲: %c  上一曲: %c", CNEXT, CPREV);
	printf("  下一页: %c  上一页: %c\n", CPAGENEXT, CPAGEPREV);
}


void PlayerUI::showLyric(const Lyric &ly)
{
	int index;
	const std::string &mid = ly.getLyric(currPosition, &index);
	
	if (index > 0) {
		Screen::mprint(ly.getLyricAt(index-1));
		/* mprint 不会自动换行 */
		printf("\n");
	} else {
		printf("\n");
	}
	
	printf("\e[1;33m");
	Screen::mprint(mid);
	printf("\e[0m\n");
	
	Screen::mprint(ly.getLyricAt(index+1));
	printf("\n");
}


void PlayerUI::printUI()
{
	currState = player.getState();
	if (currState == NOT_READY) {
		return;
	}
	
	const MusicEntry &e = data[current];
	currName = e.getName().c_str();
	currArtist = e.getArtist().c_str();
	currPosition = player.getPosition();
	currDuration = player.getDuration();
	
	SLmillisecond cur = currPosition / 1000;
	SLmillisecond dur = currDuration / 1000;
	
	Screen::clear();
	printf("%s (%d/%d): %s - %s\n", getStateString(currState), current, data.size()-1, currName, currArtist);
	printf("进度: %lu:%d/%lu:%d\n", cur/60, cur%60, dur/60, dur%60);
	
	printf("\n");
	showLyric(e.getLyric());
	
	printf("\n");
	showPlayList();
	showHelp();
}


void PlayerUI::setData(const std::vector<MusicEntry> &data)
{
	this->data = std::move(data);
	emit("start");
}


int PlayerUI::exec()
{
	finish = false;
	needShowHelp = DEFAULT_SHOW_HELP;
	needShowPlayList = DEFAULT_SHOW_PLAY_LIST;
	
	volatile bool ui = false;
	
	this->on("start", [&](void *p) {
		keyboard->start();
		current = 0;
		page = 0;
		
		if (data.size() == 0) {
			printf("未找到音乐\n");
			finish = true;
			return;
		}
		
		player.setCallback([&]() {
			this->emit("next");
		});
		
		play(0);
		ui = true;
	});
	
	this->on("pause", [&](void *p) {
		player.pause();
	});
	
	this->on("next", [&](void *p) {
		playNext();
	});
	
	this->on("prev", [&](void *p) {
		playPrev();
	});
	
	this->on("page-next", [&](void *p) {
		int max = data.size() / PAGE_ITEM;
		if (page == max) {
			page = 0;
		} else {
			page++;
		}
	});
	
	this->on("page-prev", [&](void *p) {
		int max = data.size() / PAGE_ITEM;
		if (page == 0) {
			page = max;
		} else {
			page--;
		}
	});
	
	this->on("toggle-help", [&](void *p) {
		needShowHelp = !needShowHelp;
	});
	
	this->on("toggle-play-list", [&](void *p) {
		needShowPlayList = !needShowPlayList;
	});
	
	this->on("numkey", [&](void *p) {
		if (!needShowPlayList) {
			return;
		}
		
		int key = (int) p;
		play(getIndex(page, key));
	});
	
	this->on("seek-left", [&](void *p) {
		// 5s
		SLmillisecond pos = player.getPosition();
		SLmillisecond d = 5000;
		if (pos < d) {
			pos = 0;
		} else {
			pos -= d;
		}
		player.seek(pos);
	});
	
	this->on("seek-right", [&](void *p) {
		// 5s
		SLmillisecond pos = player.getPosition() + 5000;
		player.seek(pos);
	});
	
	
	while (!finish) {
		if (ui) {
			printUI();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	
	while (!finish);
	keyboard->stop();
		
	removeEvent("pause");
	removeEvent("next");
	removeEvent("prev");
	removeEvent("seek-left");
	removeEvent("seek-right");
	removeEvent("data");
	return 0;
}

