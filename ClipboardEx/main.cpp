#include "DockableWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QResource>
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"
#include "ConfigManager.h"
#include "MainControl.h"
#include "def.h"
#ifdef UWP
#include "ROApi.h"
using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::System;
#endif

int main(int argc, char *argv[])
{
#ifdef UWP
	Windows::Foundation::Initialize();
#endif
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

	// skin
	QString themeFile = "theme.rcc";
	if (QFile(themeFile).exists()) {
		QResource::registerResource(themeFile);
	}

	MainControl::getInstance()->readConfig();
	MainControl::getInstance()->setUpUI();

	return a.exec();
}
