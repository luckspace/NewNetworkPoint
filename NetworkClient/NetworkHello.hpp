#pragma once

#include <qtcpsocket.h>

class NetworkHello final : public QTcpSocket
{
	Q_OBJECT
public:
	NetworkHello(QObject* _parent = nullptr);
signals:
	void CatchMessage(QString _msg);
public slots:
	void SocketHello();
private slots:
	void WriteMessage();

	void CatchServerMessage();
};
