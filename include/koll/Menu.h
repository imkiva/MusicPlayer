#pragma once

#include "koll/Koll.h"
#include "koll/Screen.h"
#include "koll/Keyboard.h"
#include "koll/Text.h"

namespace koll {

template <typename T>
class Menu {
public:
	class Item {
	private:
		const T item;
		std::string infoMsg;
		std::function<void(Menu<T>*, Item*, int)> listener;
		int id;
		bool hidden;
		bool clickable;
		
	public:
		Item(const T &target, const int id, const std::string &infoMsg):item(target), infoMsg(infoMsg) {
			setHidden(false);
			setClickable(true);
			setId(id);
		}
		
		virtual ~Item() {}
		
		virtual const std::string& getTitle() const {
			return item;
		}
		
		virtual bool isClickable() const {
			return clickable;
		}
		
		virtual bool isHidden() const {
			return hidden;
		}
		
		virtual const std::string& getInfoMessage() {
			return infoMsg;
		}
		
		int getId() const {
			return id;
		}
		
		void performClick(Menu<T> *menu, int position) {
			if (listener) {
				listener(menu, this, position);
			}
		}
		
		Item* setId(int id) {
			this->id = id;
			return this;
		}
		
		Item* setHidden(bool hidden) {
			this->hidden = hidden;
			return this;
		}
		
		Item* setClickable(bool clickable) {
			this->clickable = clickable;
			return this;
		}
		
		Item* setInfoMessage(const std::string &infoMsg) {
			this->infoMsg = std::move(infoMsg);
			return this;
		}
		
		Item* setListener(const std::function<void(Menu<T>*, Item*, int)> &cb) {
			listener = cb;
			return this;
		}
	};
	
private:
	std::vector<Item> items;
	std::function<bool(Menu<T>*, Item*, int)> listener;
	std::string title;
	Keyboard *keyboard;
	
	int currentItem = 0;
	int key;
	int pageMax = 6;
	
	bool showing = false;
	
	void setupKeyboard() {
		keyboard = new Keyboard();/*Keyboard::getDefault();*/
		keyboard->on('5', [&](const int &k) {
			doSelect();
		});
		
		keyboard->on('2', [&](const int &k) {
			moveUp();
		});
		
		keyboard->on('8', [&](const int &k) {
			moveDown();
		});
	}
	
	void moveDown() {
		key = 8;
		if (currentItem == items.size()-1) {
			currentItem = 0;
			return;
		}
		
		currentItem++;
	}
	
	void moveUp() {
		key = 2;
		if (currentItem == 0) {
			currentItem = items.size()-1;
			return;
		}
		
		currentItem--;
	}
	
	void doSelect() {
		Item* item = getItem(currentItem);
		bool clickable = item->isClickable();
		if (!clickable) {
			return;
		}
		
		bool callItem = true;
		if (listener) {
			callItem = !listener(this, item, currentItem);
		}
		
		if (callItem) {
			item->performClick(this, currentItem);
		}
	}
	
	int printMenu() {
		// TODO implements pageMax
		Screen::clear();
		int pos = 0;
		int shown = 0;
		bool printDot = false;
		const char *willPrint = NULL;
		
		if (!title.empty()) {
			printf("\e[1;33m");
			Screen::mprint(title);
			printf("\e[0m\n");
			
			int width = Screen::get()->getWidth();
			for (int i=0; i<width; i++) {
				putchar('-');
			}
			putchar('\n');
		}
		
		for (auto it=items.begin();
			it!=items.end(); 
			it++) 
		{
			if ((*it).isHidden()) {
				if (pos == currentItem) {
					if (key == 2) {
						moveUp();
					} else if (key == 8) {
						moveDown();
					}
				}
				pos++;
				continue;
			}
			
			putchar(' ');
			if (pos == currentItem) {
				printf("\e[033;1;5;33m>\e[033;0m");
			} else {
				putchar(' ');
			}
			putchar(' ');
			
			const std::string &info = (*it).getInfoMessage();
			
			if (!info.empty() && pos == currentItem) {
				const std::string &title = (*it).getTitle();
				int titleW = Text::getStringWidth(title) + 7; // 5 = ' > '(3) + ' (' ') '(4)
				int infoW = Text::getStringWidth(info);
				int termW = Screen::get()->getWidth();
				
				printDot = false;
				willPrint = NULL;
				
				if (titleW + infoW > termW) {
					titleW += 3; // '...'
					int pos = Text::getPartString(info, termW - titleW);
					willPrint = info.substr(0, pos).c_str();
					printDot = true;
				} else {
					willPrint = info.c_str();
				}
				
				printf("%s (%s%s) \n", title.c_str(), willPrint, (printDot ? "..." : ""));
				
			} else {
				printf("%s\n", (*it).getTitle().c_str());
			}
			
			pos++;
			shown++;
		}
		
		return shown;
	}
	
public:
	Menu() {
		setupKeyboard();
	}
	
	Menu(const std::string &title):title(title) {
		setupKeyboard();
	}
	
	~Menu() {
		delete keyboard;
	}
	
	Item* addItem(const T &item, int id = -1, const std::string &infoMsg = "") {
		if (id == -1) {
			id = items.size();
		}
		
		return addItem(Item(item, id, infoMsg));
	}
	
	Item* addItem(const Item &item) {
		items.push_back(item);
		int position = items.size() - 1;
		
		return getItem(position);
	}
	
	Item* getItem(int position) {
		if (position < 0 || position >= items.size()) {
			return NULL;
		}
		
		return &(items[position]);
	}
	
	Item* getItemById(int id) {
		for (auto it = items.begin();
					it != items.end();
					it++
					) {
					if (id == (*it).getId()) {
						return &(*it);
					}
		}
		
		return NULL;
	}
	
	void show() {
		showing = true;
		keyboard->start();
		while (showing) {
			if (printMenu() == 0) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		keyboard->stop();
	};
	
	void setListener(const std::function<bool(Menu<T>*, Item*, int)> &cb) {
		listener = cb;
	}
	
	void setPageMaxItem(int m) {
		pageMax = m;
	}
	
	void setTitle(const std::string &title) {
		this->title = std::move(title);
	}
	
	void dismiss() {
		showing = false;
	}
	
	bool isShowing() {
		return showing;
	}
};


typedef Menu<std::string> StringMenu;
typedef StringMenu::Item StringMenuItem;

}
