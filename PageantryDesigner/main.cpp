#include "TopWindow.h"
#include <QtWidgets/QApplication>
#if _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif
int main(int argc, char *argv[])
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
	QApplication a(argc, argv);
	TopWindow w;
	w.show();
	return a.exec();
}
