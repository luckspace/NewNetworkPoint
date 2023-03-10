#include "pch.h"
#include "CryptoFilters.h"

#include <cryptopp/filters.h>

luck::raii_dll<std::string> luck::Filters(const std::string& _data, TransMode _data_mode, TransMode _out_mode)
{
	using namespace CryptoPP;

	auto data_trans = GainTransformation(_data_mode);
	auto out_trans = GainTransformation(_out_mode);

	std::string data_original;
	if (data_trans != nullptr) data_trans->Detach(new StringSink(data_original));

	std::string rst;

	return rst;
}