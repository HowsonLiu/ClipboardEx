#include "DockableWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"
#include "ConfigManager.h"
#include "MainControl.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// i18n
	QLocale local;
	QString langFile = ":/i18n/res/i18n/en_us.qm";
	switch (local.language()) {
	case QLocale::Chinese: langFile = ":/i18n/res/i18n/zh_cn.qm"; break;
	default: break;
	}
	QTranslator translator;
	translator.load(langFile);
	a.installTranslator(&translator);

	MainControl::getInstance()->readConfig();
	MainControl::getInstance()->setUpUI();

	return a.exec();
}
