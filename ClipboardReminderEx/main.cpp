#include "DockableWindow.h"
#include <QtWidgets/QApplication>
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	ClipboardTipsWindow w;
	w.show();

	NumMenuActionWidget n("Show", 1, 3, 5);
	n.show();

	SliderMenuActionWidget s("Show", 1, 50, 100);
	s.show();

	return a.exec();
}
