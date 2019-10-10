#include "TopWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TopWindow w;
	w.show();
	return a.exec();
}
