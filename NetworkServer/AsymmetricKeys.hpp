#pragma once

#include "MySqlThreadPool.hpp"

class AsymmetricKeys
{
public:
private:
	AsymmetricKeys();

	luck::sql::MySqlThreadPool::SelfShare th_pool;
};
