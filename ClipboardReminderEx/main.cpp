#include "DockableWindow.h"
#include <QtWidgets/QApplication>
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"
#include "ConfigManager.h"
#include "MainControl.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainControl::getInstance()->readConfig();
	MainControl::getInstance()->setUpUI();

	return a.exec();
}
