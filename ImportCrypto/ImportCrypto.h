#pragma once

#ifdef IMPORTCRYPTO_EXPORTS
#define IMPORTCRYPTO_API __declspec(dllexport)
#else
#define IMPORTCRYPTO_API __declspec(dllimport)
#endif

#include <string>

namespace CryptoPP { class BufferedTransformation; }

namespace luck
{
	template<typename T>
	class raii_dll
	{
	public:
		raii_dll(T&& _v) : local_value(std::forward<T>(_v)) {}

		const T& operator()() { return local_value; }
	private:
		T local_value;
	};

	template class IMPORTCRYPTO_API raii_dll<std::string>;

	enum IMPORTCRYPTO_API TransMode
	{
		TransMode_Default = 0,

		TransMode_From_Hex,
		TransMode_From_Base64,

		TransMode_To_Hex,
		TransMode_To_Base64,
	};

	extern CryptoPP::BufferedTransformation* GainTransformation(TransMode _mode);
}
