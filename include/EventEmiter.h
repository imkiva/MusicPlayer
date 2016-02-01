#pragma once

#include <thread>
#include <mutex>
#include <map>
#include <functional>

namespace kiva {

typedef std::function<void(void*)> EventCallback;

template <typename T>
class EventEmiter {
private:
	std::map<T, EventCallback> event;
	std::mutex evMutex;

public:
	EventEmiter() { }
	virtual ~EventEmiter() { }
	
	void on(const T &trigger, const EventCallback &cb)
	{
		registerEvent(trigger, cb);
	}
	
	void registerEvent(const T &trigger, const EventCallback &cb)
	{
		std::lock_guard<std::mutex> lock(evMutex);
		event[trigger] = cb;
	}
	
	void removeEvent(const T &trigger)
	{
		std::lock_guard<std::mutex> lock(evMutex);
		event.erase(trigger);
	}
	
	void emit(const T &trigger, void *arg = NULL)
	{
		std::lock_guard<std::mutex> lock(evMutex);
		
		for (auto &e : event) {
			if (e.first == trigger) {
				e.second(arg);
				break;
			}
		}
	}
};

}

