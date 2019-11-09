#pragma once
#include <QObject>
#include "ClipboardTipsWindow.h"

class QSettings;

/*!
 * \class RegeditManager
 * \brief Read or write value on register
 * \author liuhaosheng
 * \date October 2019
 */
class RegeditManager : public QObject
{
	Q_OBJECT
public:
	static RegeditManager* getInstance();
	void enableRunStartUp(bool);

private:
	RegeditManager(QObject* parent = nullptr);
};

/*!
 * \class IniManager
 * \brief Read or write value on ini file
 * \author liuhaosheng
 * \date October 2019
 */
class IniManager : public QObject 
{
	Q_OBJECT
public:
	static IniManager* getInstance();
	int getHistorySize() const;
	void setHistorySize(const int);
	int getTipsListHeight() const;
	void setTipsListHeight(const int);
	QSize getTipsRectSize() const;
	void setTipsRectSize(const QSize&);
	QList<ClipboardTipsWindowState> getWindowPositions() const;
	void setWindowPositions(const QList<ClipboardTipsWindowState>&);

private:
	IniManager(QObject* parent = nullptr);

private:
	QSettings* ini;
};