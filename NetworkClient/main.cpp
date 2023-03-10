#include <qapplication.h>

#include "ConsoleWidget.hpp"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	ConsoleWidget center;
	center.show();

	return app.exec();
}
