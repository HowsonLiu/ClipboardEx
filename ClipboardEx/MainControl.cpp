#include "MainControl.h"
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"
#include "ConfigManager.h"
#include "HistoryDataList.h"
#include "def.h"
#include <QMenu>
#include <QWidgetAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

MainControl::MainControl(QObject* parent /*= nullptr*/) : QObject(parent)
	, m_tipsWindowState()
	, m_historySize(kHistorySizeDefault)
	, m_showTime(kShowTimeDefault)
{
}

MainControl* MainControl::getInstance()
{
	static MainControl mainControl;
	return &mainControl;
}

void MainControl::readConfig()
{
	m_bStartUp = RegeditManager::getInstance()->currentStartUpState();
	float showTime = IniManager::getInstance()->getShowTime();
	auto tipsWindowState = IniManager::getInstance()->getWindowPositions();
	int historySize = IniManager::getInstance()->getHistorySize();

	if (showTime > -1.0f) m_showTime = showTime;
	if (historySize) m_historySize = historySize;
	if (!tipsWindowState.isEmpty()) m_tipsWindowState = tipsWindowState;
}

void MainControl::setUpUI()
{
	setUpQss();
	setUpTrayIcon();
	setUpWindow();
}

bool MainControl::hasLanguageFont() const
{
	return QLocale().language() == QLocale::Chinese;
}

QFont MainControl::getLanguageFont() const
{
	switch (QLocale().language()) {
	case QLocale::Chinese: return QFont("SimHei");
	}
	return QFont();
}

void MainControl::setUpWindow()
{
	HistoryDataList::getInstance()->onSetListSize(m_historySize);
	if (m_tipsWindowState.isEmpty()) {
		ClipboardTipsWindow* window = new ClipboardTipsWindow;
		window->updateHistoryList();
		window->show();
		window->move(QApplication::desktop()->screen()->rect().center() - window->rect().center());
		m_tipsWindows.push_back(window);
	}
	else {
		for (auto state : m_tipsWindowState) {
			ClipboardTipsWindow* window = new ClipboardTipsWindow;
			window->loadTipsWindowState(state);
			window->show();
			m_tipsWindows.push_back(window);
		}
	}
	HistoryDataList::getInstance()->init();
}

void MainControl::setUpTrayIcon()
{
	QSystemTrayIcon* trayIcon = new QSystemTrayIcon;
	QMenu* trayIconMenu = new QMenu;
	trayIconMenu->setObjectName(kTrayMenu);
	trayIconMenu->setStyleSheet(m_menuQss);
	if (MainControl::getInstance()->hasLanguageFont())
		trayIconMenu->setFont(MainControl::getInstance()->getLanguageFont());

	// start up
	QAction* startUpAction = new QAction(trayIconMenu);
	startUpAction->setText(tr("Start up"));
	startUpAction->setCheckable(true);
	startUpAction->setObjectName(kStartUpAction);
	startUpAction->setEnabled(m_bStartUp >= 0);
	startUpAction->setChecked(m_bStartUp == 1);

	// show time
	NumMenuActionWidget* showTimeWidget = new NumMenuActionWidget(tr("Show Time"),
		kShowTimeMin, m_showTime, kShowTimeMax, kShowTimeStep, trayIconMenu);
	QWidgetAction* showTimeAction = new QWidgetAction(trayIconMenu);
	showTimeAction->setDefaultWidget(showTimeWidget);

	// history size
	NumMenuActionWidget* historySizeWidget = new NumMenuActionWidget(tr("History Size"),
		kHistorySizeMin, m_historySize, kHistroySizeMax, kHistorySizeStep, trayIconMenu);
	QWidgetAction* historySizeAction = new QWidgetAction(trayIconMenu);
	historySizeAction->setDefaultWidget(historySizeWidget);

	// number of tips windows
	NumMenuActionWidget* tipsNumWidget = new NumMenuActionWidget(tr("Num of tips"),
		kTipsNumMin, m_tipsWindowState.size(), kTipsNumMax, kTipsNumStep, trayIconMenu);
	QWidgetAction* tipsNumAction = new QWidgetAction(trayIconMenu);
	tipsNumAction->setDefaultWidget(tipsNumWidget);

	// exit
	QAction* exitAction = new QAction(trayIconMenu);
	exitAction->setText(tr("Exit"));

	trayIconMenu->addAction(startUpAction);
	trayIconMenu->addAction(showTimeAction);
	trayIconMenu->addAction(historySizeAction);
	trayIconMenu->addAction(tipsNumAction);
	trayIconMenu->addAction(exitAction);

	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/res/image/clipboardex32x32.png"));	// neccessary
	trayIcon->show();
	trayIcon->showMessage("ClipboardEx", tr("is enabled"), trayIcon->icon(), kTrayMsgShowTime);

	connect(startUpAction, &QAction::triggered, this, [this, startUpAction](bool b) {
		m_bStartUp = RegeditManager::getInstance()->enableRunStartUp(b);
		startUpAction->setEnabled(m_bStartUp >= 0);
		startUpAction->setChecked(m_bStartUp == 1);
	});
	connect(tipsNumWidget, &NumMenuActionWidget::sigNumChange, this, &MainControl::onTipsWindowNumChange);
	connect(showTimeWidget, &NumMenuActionWidget::sigNumChange, this, &MainControl::onShowTimeChanged);
	connect(historySizeWidget, &NumMenuActionWidget::sigNumChange, this, &MainControl::onHistorySizeChanged);
	connect(historySizeWidget, &NumMenuActionWidget::sigNumChange, HistoryDataList::getInstance(), &HistoryDataList::onSetListSize);
	connect(exitAction, &QAction::triggered, this, &MainControl::onSaveConfigure);
	connect(exitAction, &QAction::triggered, this, &QApplication::quit);
	connect(trayIcon, &QSystemTrayIcon::activated, this, &MainControl::onTrayActivated);
}

void MainControl::setUpQss()
{
	QString menuName = ":/qss/res/qss/menu.qss";
	QString winName = ":/qss/res/qss/window.qss";
	do {
		QFile qssFile;
		qssFile.setFileName(menuName);
		if (!qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) break;
		m_menuQss = qssFile.readAll();
		qssFile.close();

		qssFile.setFileName(winName);
		if (!qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) break;
		m_windowQss = qssFile.readAll();
		qssFile.close();
	} while (false);
}

void MainControl::onShowTimeChanged(float f)
{
	m_showTime = f;
}

void MainControl::onHistorySizeChanged(int n)
{
	m_historySize = n;
}

void MainControl::onTipsWindowNumChange(int i)
{
	while (m_tipsWindows.size() > i) {
		auto window = m_tipsWindows.front();
		window->close();
		m_tipsWindows.pop_front();
	}
	while (m_tipsWindows.size() < i) {
		ClipboardTipsWindow* window = new ClipboardTipsWindow;
		window->setStyleSheet(m_windowQss);
		if (MainControl::getInstance()->hasLanguageFont())
			window->setFont(MainControl::getInstance()->getLanguageFont());
		window->move(QApplication::desktop()->screen()->rect().center() - window->rect().center());
		window->show();
		window->updateHistoryList();
		m_tipsWindows.push_back(window);
	}
}

void MainControl::onSaveConfigure()
{
	QList<ClipboardTipsWindowState> states;
	for (auto tip : m_tipsWindows)
		states.push_back(tip->getTipsWindowState());
	IniManager::getInstance()->setWindowPositions(states);
	IniManager::getInstance()->setHistorySize(m_historySize);
	IniManager::getInstance()->setShowTime(m_showTime);
}

void MainControl::onTrayActivated(QSystemTrayIcon::ActivationReason reson)
{
	if (reson != QSystemTrayIcon::DoubleClick) return;
	for (auto& tips : m_tipsWindows)
		tips->show();
}
