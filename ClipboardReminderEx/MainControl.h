#pragma once
#include "ClipboardTipsWindow.h"
#include <QList>

class ClipboardTipsWindow;

/*!
 * \class MainControl
 * \brief Control the application
 * \author liuhaosheng
 * \date November 2019
 */
class MainControl : public QObject
{
	Q_OBJECT
public:
	MainControl(QObject* parent = nullptr);
	void readConfig();
	void setUpUI();
	void setAutoSave();

private:
	void setUpWindow();
	void setUpTrayIcon();
	void setUpQss();

private slots:
	void onTipsWindowNumChange(int);
	void onSaveConfigure();

private:
	QList<ClipboardTipsWindow*> m_tipsWindows;

	// config
	QList<ClipboardTipsWindowState> m_tipsWindowState;
	int m_historySize;
	//int m_tipsListHeight;
	//QSize m_tipsRectSize;

	// qss
	QString m_menuQss;
	QString m_windowQss;
};

