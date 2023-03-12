#pragma once

#include <qwidget.h>
#include <qplaintextedit.h>
#include <qboxlayout.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>

#include <qlabel.h>

class NetworkSocket : public QObject
{
	Q_OBJECT
public:
	NetworkSocket(QTcpSocket* _bind, QObject* _parent = nullptr);
signals:
	void CatchMessage(QString _msg);
private slots:
	void MsgReady();
private:
	QTcpSocket* bind_socket;
};

class ConsoleWidget : public QWidget
{
	Q_OBJECT
public:
	ConsoleWidget(QWidget* _parent = nullptr);
private:
	QPlainTextEdit* output_edit;

	QVBoxLayout* center_box;

	//	network
	QTcpServer* tcp_server;

	//	mysql status
	QLabel* sqlserver_title;
	QLabel* sqlserver_pixmap;

	QHBoxLayout* sqlserver_box;
};
