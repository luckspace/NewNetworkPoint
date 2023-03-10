#include "NetworkHello.hpp"

NetworkHello::NetworkHello(QObject* _parent) : QTcpSocket(_parent)
{
	connect(this, &QTcpSocket::connected, this, &NetworkHello::WriteMessage);
	connect(this, &QTcpSocket::readyRead, this, &NetworkHello::CatchServerMessage);

	connect(this, &QTcpSocket::errorOccurred,
		[=](QTcpSocket::SocketError _error)
		{
			emit CatchMessage("socket error code " + QByteArray::number(_error));
		}
	);
}

void NetworkHello::SocketHello()
{
	switch (state())
	{
	case QTcpSocket::UnconnectedState:
		connectToHost("127.0.0.1", 10032);
		break;
	case QTcpSocket::ConnectedState:
		WriteMessage();
		break;
	}
}

#include <qfile.h>
//	std
#include <string>
//	json
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
//	crypto
#include <CryptoEcc.h>

void NetworkHello::WriteMessage()
{
	try
	{
		QJsonObject json_ecc;
		std::string ecc_pub_key_hex;
		{
			QFile file_json(":/PubResource/ServerEccPublicKey.json");
			if (!file_json.open(QIODevice::ReadOnly))
				throw std::runtime_error("QFile not found");

			QJsonObject json_root = QJsonDocument::fromJson(file_json.readAll()).object();
			ecc_pub_key_hex = json_root["key_pub"].toString().toStdString();

			json_ecc["index"] = json_root["key_map"];
		}

		{

			std::string original_data{ "hello server" };
			std::string data_en_base = luck::EciesEncode(ecc_pub_key_hex, original_data)();

			json_ecc["local"] = QString::fromStdString(data_en_base);
		}

		auto hello_bytes = QJsonDocument(json_ecc).toJson(QJsonDocument::Compact);
		write(hello_bytes);
	}
	catch (std::exception& _e)
	{
		emit CatchMessage(_e.what());
	}
}

void NetworkHello::CatchServerMessage()
{
	emit CatchMessage(readAll());
}
