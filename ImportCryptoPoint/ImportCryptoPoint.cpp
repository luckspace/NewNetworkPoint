#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ImportCryptoPoint
{
	TEST_CLASS(ImportCryptoPoint)
	{
	public:

		TEST_METHOD(CryptoEcies)
		{
			using namespace CryptoPP;

			std::string ecies_pri_key = luck::EciesPriKey()();
			std::string ecies_pub_key = luck::EciesPubKey(ecies_pri_key)();

			std::string original_data = "hello ecies, the is str.";
			std::string data_en_base, data_de_original;

			data_en_base = luck::EciesEncode(ecies_pub_key, original_data)();
			data_de_original = luck::EciesDecode(ecies_pri_key, data_en_base)();

			Assert::AreEqual(original_data.c_str(), data_de_original.c_str());
		}
	};
}
