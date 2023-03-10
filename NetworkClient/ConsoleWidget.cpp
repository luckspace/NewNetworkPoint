#include "ConsoleWidget.hpp"

#include "NetworkHello.hpp"

ConsoleWidget::ConsoleWidget(QWidget* _parent)
{

	output_edit = new QPlainTextEdit(this);
	output_edit->setReadOnly(true);

	tool_group = new QGroupBox("tools", this);
	//	>>> tools component
	//		>>> network hello
	auto tool_network_hello = new NetworkHello(this);
	tools.push_back(tool_network_hello);
	hello_btn = new QPushButton("network hello", this);
	hello_btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(hello_btn, &QPushButton::released, tool_network_hello, &NetworkHello::SocketHello);
	connect(tool_network_hello, &NetworkHello::CatchMessage, output_edit, &QPlainTextEdit::appendPlainText);
	//	>>> tools box
	tools_box = new QGridLayout(this);
	tools_box->addWidget(hello_btn, 0, 0);
	//	>>> tools group setting
	tool_group->setLayout(tools_box);

	center_box = new QVBoxLayout(this);
	center_box->addWidget(output_edit);
	center_box->addWidget(tool_group);

	setWindowTitle("console widget");
	resize(800, 600);
	setLayout(center_box);
}
