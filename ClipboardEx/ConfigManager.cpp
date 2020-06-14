#include "ConfigManager.h"
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

int RegeditManager::enableRunStartUp(bool b)
{
#ifdef UWP
	// https://blog.csdn.net/eiilpux17/article/details/78964795
	auto asynOp = StartupTask::GetAsync(ref new Platform::String(TEXT("ClipboardExStartUpId")));
	if (asynOp == nullptr) {
		qDebug() << "UWP Start up API error";
		return -1;
	}
	auto startUpTask = concurrency::create_task(asynOp).get();
	if (b) {
		auto state = concurrency::create_task(startUpTask->RequestEnableAsync()).get();
		qDebug() << "UWP enable start up state: " << static_cast<int>(state);
		return state == StartupTaskState::Enabled || state == StartupTaskState::EnabledByPolicy ? 1 : 0;
	}
	else {
		startUpTask->Disable();
		return 0;
	}
#else
	QString appName = QApplication::applicationName();
	QString appPath = QApplication::applicationFilePath();
	if (b) {
		QString oldPath = m_reg->value(appName).toString();
		if (oldPath != appPath) m_reg->setValue(appName, appPath);
		return true;
	}
	else {
		m_reg->remove(appName);
		return false;
	}
	return true;
#endif
}

int RegeditManager::currentStartUpState() const
{
#ifdef UWP
	auto asynOp = StartupTask::GetAsync(ref new Platform::String(TEXT("ClipboardExStartUpId")));
	if (!asynOp) {
		qDebug() << "UWP Start up API error";
		return -1;
	}
	auto startUpTask = concurrency::create_task(asynOp).get();
	qDebug() << "UWP enable start up state: " << static_cast<int>(startUpTask->State);
	return startUpTask->State == StartupTaskState::Enabled || startUpTask->State == StartupTaskState::EnabledByPolicy ? 1 : 0;
#else
	QString appName = QApplication::applicationName();
	QString appPath = QApplication::applicationFilePath();
	QString regPath = m_reg->value(appName).toString();
	qDebug() << "Regedit startup path: " << regPath;
	return regPath == appPath;
#endif
}

RegeditManager::RegeditManager(QObject* parent /*= nullptr*/) : QObject(parent)
{
#ifndef UWP
	m_reg = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
#endif
}

IniManager* IniManager::getInstance()
{
	static IniManager instance;
	return &instance;
}

float IniManager::getShowTime() const
{
	auto value = ini->value("show_time");
	if (value.isNull()) return -1.0f;
	return value.toFloat();
}

void IniManager::setShowTime(float showTime)
{
	ini->setValue("show_time", showTime);
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
