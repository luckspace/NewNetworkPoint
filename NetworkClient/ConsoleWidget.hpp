#pragma once

#include <qwidget.h>
#include <qpushbutton.h>
#include <qplaintextedit.h>
#include <qboxlayout.h>
#include <qgroupbox.h>

class ConsoleWidget : public QWidget
{
	Q_OBJECT
public:
	ConsoleWidget(QWidget* _parent = nullptr);
private:
	QPlainTextEdit* output_edit;

	QGroupBox* tool_group;
	QVector<QObject*> tools;
	QGridLayout* tools_box;
	QPushButton* hello_btn;

	QVBoxLayout* center_box;
};
