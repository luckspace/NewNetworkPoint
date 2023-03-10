#pragma once

#include "ImportCrypto.h"

#include <string>

namespace luck
{
	extern IMPORTCRYPTO_API raii_dll<std::string> EciesPriKey(TransMode _out_mode = TransMode_To_Hex);

	extern IMPORTCRYPTO_API raii_dll<std::string> EciesPubKey(const std::string& _ecies_pri_key, TransMode _key_mode = TransMode_From_Hex, TransMode _out_mode = TransMode_To_Hex);

	extern IMPORTCRYPTO_API raii_dll<std::string> EciesEncode(
		const std::string& _key, const std::string& _buff,
		TransMode _key_mode = TransMode_From_Hex, TransMode _buff_mode = TransMode_Default, TransMode _out_mode = TransMode_To_Base64
	);

	extern IMPORTCRYPTO_API raii_dll<std::string> EciesDecode(
		const std::string& _key, const std::string& _buff,
		TransMode _key_mode = TransMode_From_Hex, TransMode _buff_mode = TransMode_From_Base64, TransMode _out_mode = TransMode_Default
	);
}
