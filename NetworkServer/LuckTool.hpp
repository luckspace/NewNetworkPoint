#pragma once

template<typename T>
class Singleton
{
public:
	static T* Instance()
	{
		static T sta_singleton;
		return &sta_singleton;
	}
};
