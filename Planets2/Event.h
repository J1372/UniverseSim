#pragma once
#include <functional>
#include <limits>
#include <utility>
#include <algorithm>

// Represents an observable event, that objects can subscribe a method to. 
template <class ParamType>
class Event {

	int next_id = std::numeric_limits<int>::min();
	std::vector<std::pair<int, std::function<void(ParamType)>>> observers;

public:

	// Adds observer callback and returns its id.
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
	int ret_id = next_id;
	next_id++;
	return ret_id;
}

template<class ParamType>
inline void Event<ParamType>::rem_observer(int id)
{
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
