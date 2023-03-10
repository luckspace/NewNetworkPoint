#pragma once

#include "pch.h"
#include "ImportCrypto.h"

namespace luck
{
	extern IMPORTCRYPTO_API raii_dll<std::string> Filters(const std::string& _data, TransMode _data_mode, TransMode _out_mode);
}
