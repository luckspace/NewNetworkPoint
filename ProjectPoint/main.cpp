#include <iostream>

#include <string>
#include <vector>
#include <map>

#include <ranges>

#include "ThreadPool.hpp"

class A
{
public:
	void fun(int a)
	{
		std::cout << a << std::endl;
	}
};

int main()
{
	LuckSpace::ThreadPool th_pool;

	A a;
	th_pool.Submit(&A::fun, &a, 10);
}
