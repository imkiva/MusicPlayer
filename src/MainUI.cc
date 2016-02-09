#include <time.h>
#include <stdlib.h>

#include <iostream>

#include "Control.h"
#include "MainUI.h"
#include "Thread.h"

#ifdef ANDROID
#	include "OpenSLESMusicPlayer.h"
#else
#	include "SDL2MusicPlayer.h"
#endif

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


static const char* getModeString(PlayMode s)
{
	switch(s) {
	case LOOP_ALL:
		return "列表循环";
	case LOOP_ONE:
		return "单曲循环";
	case RANDOM:
		return "随机播放";
	}
}


MainUI::MainUI()
	:lyDownloader(new CloudMusicApi())
{

#ifdef ANDROID
	player = new OpenSLESMusicPlayer();
#else
	player = new SDL2MusicPlayer();
#endif
	
	cfg = Config::get();
	
	playMode = cfg->getPlayMode();
	needShowHelp = cfg->showHelp();
	needShowPlayList = cfg->showPlayList();
	current = cfg->getCurrentItem();
	page = cfg->getCurrentPage();
	currPosition = cfg->getCurrentPosition();
	
	finish = false;
	ui = false;
	srand(time(NULL));
	
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
	
	keyboard->on(CMODE, [&](const int &key) {
		this->emit("toggle-mode");
	});
	
	keyboard->on('0', '9', [&](const int &key) {
		int k = key - '0';
		this->emit("numkey", (void*) &k);
	});
	
	keyboard->on(CSEEKL, [&](const int &key) {
		this->emit("seek-left");
	});
	
	keyboard->on(CSEEKR, [&](const int &key) {
		this->emit("seek-right");
	});
	
	setupEvent();
}


MainUI::~MainUI()
{
	cfg->setPlayMode(playMode);
	cfg->setShowHelp(needShowHelp);
	cfg->setShowPlayList(needShowPlayList);
	cfg->setCurrentItem(current);
	cfg->setCurrentPage(page);
	cfg->setCurrentPosition(currPosition);
	
	delete player;
}


void MainUI::play(int pos)
{
	if (pos < 0 || pos >= data.size()) {
		return;
	}
	
	current = pos;
	const MusicEntry &e = data[pos];
	
	if (!e.getLyric().hasData()) {
		lyDownloader.downloadLyric(&(data[pos]));
	}
	
	if (!player->setSource(e.getPath())) {
		playNext();
	}
	
	player->play();
}


void MainUI::playNext()
{
	if (current == data.size()-1) {
		play(0);
	} else {
		play(current+1);
	}
}


void MainUI::playPrev()
{
	if (current == 0) {
		play(data.size()-1);
	} else {
		play(current-1);
	}
}


int MainUI::getIndex(int page, int i)
{
	return page * PAGE_ITEM + i;
}


void MainUI::showPlayList() {
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


void MainUI::showHelp() {
	if (!needShowHelp) {
		return;
	}
	
	PlayState s = player->getState();
	
	printf("操作方式: \n");
	printf(" %s: %c  退出: %c", (s == PAUSED ? "继续" : "暂停"), CPAUSE, CEXIT);
	printf("  切换帮助: %c  %s: %c  播放模式: %c\n", CHELP, (needShowPlayList ? "隐藏列表" : "显示列表"), CPLAYLIST, CMODE);
	
	printf(" 下一曲: %c  上一曲: %c", CNEXT, CPREV);
	printf("  下一页: %c  上一页: %c\n", CPAGENEXT, CPAGEPREV);
}


void MainUI::showLyric(const Lyric &ly)
{
	if (!ly.hasData()) {
		return;
	}
	
	printf("\n");
	
	/* 最大拓展的歌词显示行数 */
	/* 即最多显示前后 overshow 句歌词 */
	int overshow = 1;
	
	int index;
	const std::string &mid = ly.getLyric(currPosition, &index);
	
	if (!needShowPlayList && !needShowHelp) {
		overshow = 5;
	}
	
	for (int i=overshow; i>0; i--) {
		Screen::mprint(ly.getLyricAt(index-i));
		putchar('\n'); /* mprint 不会自动换行 */
	}
	
	printf("\e[1;33m");
	Screen::mprint(mid);
	printf("\e[0m\n");
	
	for (int i=1; i<=overshow; i++) {
		Screen::mprint(ly.getLyricAt(index+i));
		putchar('\n');
	}
}


void MainUI::showProgress()
{
	int w = Screen::get()->getWidth() - 2;
	int p = 1.0 * currPosition / currDuration * w;
	
	printf("[");
	for (int i=0; i<w; i++) {
		if (i <= p) {
			printf("=");
		} else {
			printf(" ");
		}
	}
	printf("]\n");
}


void MainUI::printUI()
{
	currState = player->getState();
	if (currState == NOT_READY) {
		return;
	}
	
	const MusicEntry &e = data[current];
	currName = e.getName().c_str();
	currArtist = e.getArtist().c_str();
	currPosition = player->getPosition();
	currDuration = player->getDuration();
	
	Millisecond cur = currPosition / 1000;
	Millisecond dur = currDuration / 1000;
	
	Screen::clear();
	printf("%s (%d/%d): %s - %s\n", getStateString(currState), current, data.size()-1, currName, currArtist);
	printf("播放模式: %s  进度: %02lu:%02d/%02lu:%02d\n", getModeString(playMode), cur/60, cur%60, dur/60, dur%60);
	
	showProgress();
	showLyric(e.getLyric());
	
	printf("\n");
	showPlayList();
	showHelp();
}


void MainUI::setupEvent()
{
	this->on("start", [&](void *p) {
		keyboard->start();
		
		if (data.size() == 0) {
			printf("未找到音乐\n");
			finish = true;
			return;
		}
		
		player->on("finish", [&](void *p) {
			switch(this->playMode) {
			case LOOP_ALL:
				this->emit("next");
				break;
			
			case LOOP_ONE:
				this->play(current);
				break;
			
			case RANDOM:
				play(rand() % data.size());
				break;
			}
		});
		
		if (current < 0 || current >= data.size()) {
			current = 0;
		}
		
		int pageMax = data.size() / PAGE_ITEM;
		if (page < 0 || page >= pageMax) {
			page = 0;
		}
		
		play(current);
		player->seek(currPosition);
		ui = true;
	});
	
	this->on("pause", [&](void *p) {
		player->pause();
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
	
	this->on("toggle-mode", [&](void *p) {
		switch(this->playMode) {
		case LOOP_ALL:
			playMode = LOOP_ONE;
			break;
		
		case LOOP_ONE:
			playMode = RANDOM;
			break;
		
		case RANDOM:
			playMode = LOOP_ALL;
			break;
		}
	});
	
	this->on("numkey", [&](void *p) {
		if (!needShowPlayList) {
			return;
		}
		
		int key = *((int*) p);
		play(getIndex(page, key));
	});
	
	this->on("seek-left", [&](void *p) {
		// 5s
		Millisecond pos = player->getPosition();
		Millisecond d = 5000;
		if (pos < d) {
			pos = 0;
		} else {
			pos -= d;
		}
		player->seek(pos);
	});
	
	this->on("seek-right", [&](void *p) {
		// 5s
		Millisecond pos = player->getPosition() + 5000;
		player->seek(pos);
	});
}


void MainUI::setData(const std::vector<MusicEntry> &data)
{
	this->data = std::move(data);
	emit("start");
}


int MainUI::exec()
{
	while (!finish) {
		if (ui) {
			printUI();
			THREAD_SLEEP(100);
		}
	}
	
	ui = false;
	keyboard->stop();
		
	removeEvent("pause");
	removeEvent("next");
	removeEvent("prev");
	removeEvent("seek-left");
	removeEvent("seek-right");
	removeEvent("data");
	
	Screen::clear();
	return 0;
}

