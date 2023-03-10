// ImportCrypto.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "ImportCrypto.h"

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>

CryptoPP::BufferedTransformation* luck::GainTransformation(TransMode _mode)
{
	using namespace CryptoPP;
	switch (_mode)
	{
	case TransMode_From_Hex:		return new HexDecoder;
	case TransMode_From_Base64:		return new Base64Decoder;

	case TransMode_To_Hex:			return new HexEncoder;
	case TransMode_To_Base64:		return new Base64Encoder;

	case TransMode_Default:
	default:
		return nullptr;
	}
}
