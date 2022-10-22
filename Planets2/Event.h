#pragma once
#include <functional>
#include <limits>
#include <utility>
#include <algorithm>

/* 
* Represents an observable event.
* Methods can be subscribed to this event.
* When the event is triggered (by notify_all),
* subscribed methods are called with a message of ParamType. 
*/
template <class ParamType>
class Event {

	int next_id = std::numeric_limits<int>::min();
	std::vector<std::pair<int, std::function<void(ParamType)>>> observers;

	// Currently, anything that subscribes a method is in charge of
	// storing its associated listener id and remembering to call rem_observer.
	// 
	// Could have a ListenerHandle, whose destructor would remove itself from the observer pool.


public:

	// Adds observer callback and returns its id.
	// It is necessary to store this id in order to detach the subscribed
	// method from the observer pool when required.
	int add_observer(std::function<void(ParamType)> observer);

	// Removes the observer callback associated with the given id.
	void rem_observer(int id);

	// Calls all callback methods with the given message parameter.
	void notify_all(ParamType message);

};

template<class ParamType>
inline int Event<ParamType>::add_observer(std::function<void(ParamType)> observer)
{
	observers.emplace_back(next_id, observer);
	int ret_id = next_id++;
	return ret_id;
}

template<class ParamType>
inline void Event<ParamType>::rem_observer(int id)
{
	// Could use lower_bound bin search, or even a map.
	// But currently, number of observers is pretty low.
	auto it = std::find_if(observers.begin(), observers.end(), [id](const std::pair<int, std::function<void(ParamType)>>& pair) {
		return pair.first == id;
	});

	observers.erase(it);
}

template<class ParamType>
inline void Event<ParamType>::notify_all(ParamType message)
{
	for (auto& pair : observers) {
		auto& observer = pair.second;
		observer(message);
	}
}
