#pragma once
template <typename T>
class Singleton
{
public :
	static T& GetInstance()
	{
		static T m_instance;
		return m_instance;
	}

protected:
	Singleton() {}

public:
	Singleton(Singleton const &) = delete;
	Singleton& operator=(Singleton const&) = delete;
};

