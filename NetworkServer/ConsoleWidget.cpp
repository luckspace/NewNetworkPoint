#include "ConsoleWidget.hpp"

ConsoleWidget::ConsoleWidget(QWidget* _parent) : QWidget(_parent)
{

	output_edit = new QPlainTextEdit(this);

	center_box = new QVBoxLayout(this);
	center_box->addWidget(output_edit);

	setWindowTitle("server console");
	resize(800, 600);
	setLayout(center_box);

	tcp_server = new QTcpServer(this);

	connect(tcp_server, &QTcpServer::newConnection,
		[=]()
		{
			auto item = new NetworkSocket(tcp_server->nextPendingConnection(), this);
			connect(item, &NetworkSocket::CatchMessage, output_edit, &QPlainTextEdit::appendPlainText);
		}
	);

	tcp_server->listen(QHostAddress::Any, 10032);
}

#include <memory>

#include <qjsondocument.h>
#include <qjsonobject.h>

#include <mysql/jdbc.h>

#include <CryptoEcc.h>

NetworkSocket::NetworkSocket(QTcpSocket* _bind, QObject* _parent) : QObject(_parent), bind_socket(_bind)
{
	connect(bind_socket, &QTcpSocket::readyRead, this, &NetworkSocket::MsgReady);
}

void NetworkSocket::MsgReady()
{
	try
	{
		std::string msg_index, msg_local;
		{
			QJsonObject json_root = QJsonDocument::fromJson(bind_socket->readAll()).object();

			msg_index = json_root["index"].toString().toStdString();
			msg_local = json_root["local"].toString().toStdString();
		}

		std::string ecies_pri_key_hex;
		{
			std::unique_ptr<sql::Connection> sql_con(sql::mysql::get_mysql_driver_instance()->connect("tcp://127.0.0.1:3306", "root", "356231064"));
			sql_con->setSchema("localpoint");
			std::unique_ptr<sql::PreparedStatement> sql_smt(sql_con->prepareStatement("select `ecc_pri_key` from `ecc_key` where `key_sha` = ?"));
			sql_smt->setString(1, msg_index);
			std::unique_ptr<sql::ResultSet> sql_rst(sql_smt->executeQuery());

			while (sql_rst->next())
			{
				ecies_pri_key_hex = sql_rst->getString(1);
			}
		}

		std::string de_str = luck::EciesDecode(ecies_pri_key_hex, msg_local)().c_str();

		emit CatchMessage(QString("hello message:"));
		emit CatchMessage(QString::fromStdString(de_str));

		//std::string status{ "socket hello-package ok;" };
		//status += bind_socket->peerAddress().toString().toStdString() + ";";

		//Singleton<MySqlManager>::Instance()->aerfa.Upload(MySqlLogData_Aerfa("log", 1, status, "ecies ok", std::chrono::system_clock::now().time_since_epoch().count()));
	}
	catch (sql::SQLException& _e)
	{
		emit CatchMessage(_e.what());
	}
	catch (std::exception& _e)
	{
		emit CatchMessage(_e.what());
	}
}
