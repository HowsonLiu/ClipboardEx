#include "DockableWindow.h"
#include <QtWidgets/QApplication>
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"
#include "ConfigManager.h"
#include "MainControl.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MainControl m;
	m.readConfig();
	m.setUpUI();
	m.setAutoSave();
	//DockableWindow w;
	//w.show();

	return a.exec();
}
