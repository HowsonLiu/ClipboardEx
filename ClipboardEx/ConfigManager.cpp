#include "ConfigManager.h"
#include <QSettings>
#include <QApplication>
#include <QDebug>

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
	// TODO need UAC
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
}

bool RegeditManager::currentStartUpState() const
{
	QString appName = QApplication::applicationName();
	QString appPath = QApplication::applicationFilePath();
	QString regPath = m_reg->value(appName).toString();
	qDebug() << "Regedit startup path: " << regPath;
	return regPath == appPath;
}

RegeditManager::RegeditManager(QObject* parent /*= nullptr*/) : QObject(parent)
{
	m_reg = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
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
