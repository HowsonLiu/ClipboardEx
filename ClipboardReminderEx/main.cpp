#include "DockableWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DockableWindow w;
	w.show();
	return a.exec();
}
