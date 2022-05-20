#pragma once
#include <functional>


// Represents an observable event, that objects can subscribe a method to. 
template <class ParamType>
class Event {

	std::vector<std::function<void(ParamType)>> observers;

public:

	void add_observer(std::function<void(ParamType)> observer);
	void rem_observer(std::function<void(ParamType)> observer);

	void notify_all(ParamType message);

};

template<class ParamType>
inline void Event<ParamType>::add_observer(std::function<void(ParamType)> observer)
{
	observers.push_back(observer);
}

template<class ParamType>
inline void Event<ParamType>::rem_observer(std::function<void(ParamType)> observer)
{
	auto it = std::find(observers.begin() observers.end(), observer);
	observers.erase(it);
}

template<class ParamType>
inline void Event<ParamType>::notify_all(ParamType message)
{
	for (auto& observer : observers) {
		observer(message);
	}
}
