
#include "pch.h"
#include "CryptoEcc.h"

#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/oids.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>

luck::raii_dll<std::string> luck::EciesPriKey(TransMode _out_mode)
{
	using namespace CryptoPP;

	auto out_trans = GainTransformation(_out_mode);

	AutoSeededRandomPool auto_rand_pool;
	ECIES<ECP>::PrivateKey ecies_pri_key;
	ecies_pri_key.Initialize(auto_rand_pool, ASN1::secp256k1());

	std::string out_buff;
	ecies_pri_key.Save(StringSink(out_buff).Ref());
	if (out_trans == nullptr) return out_buff;
	std::string rst;
	out_trans->Detach(new StringSink(rst));
	StringSource(out_buff, true, out_trans);
	return rst;
}

luck::raii_dll<std::string> luck::EciesPubKey(const std::string& _ecies_pri_key, TransMode _key_mode, TransMode _out_mode)
{
	using namespace CryptoPP;

	auto key_trans = GainTransformation(_key_mode);
	auto out_trans = GainTransformation(_out_mode);

	ECIES<ECP>::PrivateKey	ecies_pri_key;
	ECIES<ECP>::PublicKey	ecies_pub_key;
	ecies_pri_key.Load(StringSource(_ecies_pri_key, true, key_trans).Ref());
	ecies_pri_key.MakePublicKey(ecies_pub_key);

	std::string rst;
	if (out_trans == nullptr) ecies_pub_key.Save(StringSink(rst).Ref());
	else
	{
		out_trans->Detach(new StringSink(rst));
		ecies_pub_key.Save(out_trans->Ref());
	}
	return rst;
}

luck::raii_dll<std::string> luck::EciesEncode(const std::string& _key, const std::string& _buff, TransMode _key_mode, TransMode _buff_mode, TransMode _out_mode)
{
	using namespace CryptoPP;

	auto key_trans = GainTransformation(_key_mode);
	auto buff_trans = GainTransformation(_buff_mode);
	auto out_trans = GainTransformation(_out_mode);

	std::string original_buff;
	if (buff_trans != nullptr)
	{
		buff_trans->Detach(new StringSink(original_buff));
		StringSource(_buff, true, buff_trans);
	}
	else original_buff = _buff;

	std::string output_buff;
	RandomPool rand_pool;
	ECIES<ECP>::Encryptor ecies_en(StringSource(_key, true, key_trans).Ref());
	output_buff.resize(ecies_en.CiphertextLength(original_buff.size()));
	ecies_en.Encrypt(rand_pool, (byte*)original_buff.data(), original_buff.size(), (byte*)output_buff.data());

	if (out_trans != nullptr)
	{
		std::string rst;
		out_trans->Detach(new StringSink(rst));
		StringSource(output_buff, true, out_trans);
		return rst;
	}
	else return output_buff;
}

luck::raii_dll<std::string> luck::EciesDecode(const std::string& _key, const std::string& _buff, TransMode _key_mode, TransMode _buff_mode, TransMode _out_mode)
{
	using namespace CryptoPP;

	auto key_trans = GainTransformation(_key_mode);
	auto buff_trans = GainTransformation(_buff_mode);
	auto out_trans = GainTransformation(_out_mode);

	std::string original_buff;
	if (buff_trans != nullptr)
	{
		buff_trans->Detach(new StringSink(original_buff));
		StringSource(_buff, true, buff_trans);
	}
	else original_buff = _buff;

	std::string output_buff;
	RandomPool rand_pool;
	ECIES<ECP>::Decryptor ecies_de(StringSource(_key, true, key_trans).Ref());
	output_buff.resize(ecies_de.CiphertextLength(original_buff.size()));
	ecies_de.Decrypt(rand_pool, (byte*)original_buff.data(), original_buff.size(), (byte*)output_buff.data());

	if (out_trans != nullptr)
	{
		std::string rst;
		out_trans->Detach(new StringSink(rst));
		StringSource(output_buff, true, out_trans);
		return rst;
	}
	else return output_buff;
}
