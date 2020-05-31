#include "ConfigManager.h"
#include "def.h"
#include <QSettings>
#include <QApplication>
#include <QDebug>
#ifdef UWP
#include <ppltasks.h>
using namespace Windows::ApplicationModel;
using namespace concurrency;
#endif

namespace {
	static const QString g_iniFileName = "\\CREx";
}

RegeditManager* RegeditManager::getInstance()
{
	static RegeditManager instance;
	return &instance;
}

bool RegeditManager::enableRunStartUp(bool b)
{
#ifdef UWP
	// https://blog.csdn.net/eiilpux17/article/details/78964795
	auto asynOp = StartupTask::GetAsync(ref new Platform::String(TEXT("ClipboardExStartUpId")));
	if (asynOp == nullptr) {
		qDebug() << "UWP Start up API error";
		return false;
	}
	auto startUpTask = concurrency::create_task(asynOp).get();
	if (b) {
		auto state = concurrency::create_task(startUpTask->RequestEnableAsync()).get();
		qDebug() << "UWP enable start up state: " << static_cast<int>(state);
		return state == StartupTaskState::Enabled || state == StartupTaskState::EnabledByPolicy;
	}
	else {
		startUpTask->Disable();
		return false;
	}
#else

	QSettings* reg = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	QString appName = QApplication::applicationName();
	QString appPath = QApplication::applicationFilePath();
	if (b) {
		QString oldPath = reg->value(appName).toString();
		if (oldPath != appPath) reg->setValue(appName, appPath);
	}
	else {
		reg->remove(appName);
	}
	reg->deleteLater();
#endif
}

RegeditManager::RegeditManager(QObject* parent /*= nullptr*/) : QObject(parent)
{
}

IniManager* IniManager::getInstance()
{
	static IniManager instance;
	return &instance;
}

int IniManager::getHistorySize() const
{
	return ini->value("history_size").toInt();
}

void IniManager::setHistorySize(const int size)
{
	ini->setValue("history_size", size);
}

int IniManager::getTipsListHeight() const
{
	return ini->value("tips_list_height").toInt();
}

void IniManager::setTipsListHeight(const int height)
{
	ini->setValue("tips_list_height", height);
}

QSize IniManager::getTipsRectSize() const
{
	QSize res;
	res.setWidth(ini->value("tips_rect/width").toInt());
	res.setHeight(ini->value("tips_rect/height").toInt());
	return res;
}

void IniManager::setTipsRectSize(const QSize& size)
{
	ini->setValue("tips_rect/width", size.width());
	ini->setValue("tips_rect/height", size.height());
}

QList<ClipboardTipsWindowState> IniManager::getWindowPositions() const
{
	QList<ClipboardTipsWindowState> res;
	QString posstr = ini->value("old_tips_state").toString();
	for (QString str : posstr.split('|'))
		res.push_back(str);
	return res;
}

void IniManager::setWindowPositions(const QList<ClipboardTipsWindowState>& positions)
{
	QString posstr;
	for (ClipboardTipsWindowState pos : positions)
		posstr += pos + '|';
	ini->setValue("old_tips_state", posstr.left(posstr.size() - 1));
}

IniManager::IniManager(QObject* parent /*= nullptr*/) : QObject(parent)
{
	ini = new QSettings(QApplication::applicationDirPath() + g_iniFileName, QSettings::IniFormat, this);
}
