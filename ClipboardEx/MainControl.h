#pragma once
#include "ClipboardTipsWindow.h"
#include <QList>
#include <QSystemTrayIcon>

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
	static MainControl* getInstance();
	void readConfig();
	void setUpUI();

	inline QString getMenuQss() const { return m_menuQss; };
	inline QString getWindowQss() const { return m_windowQss; };
	inline float getShowTime() const { return m_showTime; }
	bool hasLanguageFont() const;
	QFont getLanguageFont() const;

private:
	MainControl(QObject* parent = nullptr);
	void setUpWindow();
	void setUpTrayIcon();
	void setUpQss();

private slots:
	void onShowTimeChanged(float);
	void onHistorySizeChanged(int);
	void onTipsWindowNumChange(int);
	void onSaveConfigure();
	void onTrayActivated(QSystemTrayIcon::ActivationReason reson);

private:
	QList<ClipboardTipsWindow*> m_tipsWindows;

	// config
	QList<ClipboardTipsWindowState> m_tipsWindowState;
	int m_historySize;
	float m_showTime;
	bool m_bStartUp;

	// qss
	QString m_menuQss;
	QString m_windowQss;
};

