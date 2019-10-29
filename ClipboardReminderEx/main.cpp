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

	DoubleSliderMenuActionWidget s("Size", "width", 50, 80, 100, "height", 1, 50, 100);
	s.show();

	return a.exec();
}
