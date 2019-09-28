#include "ReminderWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ReminderWindow w;
	w.show();
	return a.exec();
}
