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
{
}

MainControl* MainControl::getInstance()
{
	static MainControl mainControl;
	return &mainControl;
}

void MainControl::readConfig()
{
	auto tipsWindowState = IniManager::getInstance()->getWindowPositions();
	int historySize = IniManager::getInstance()->getHistorySize();

	if (historySize) m_historySize = historySize;
	if (!tipsWindowState.isEmpty()) m_tipsWindowState = tipsWindowState;
}

void MainControl::setUpUI()
{
	setUpQss();
	setUpTrayIcon();
	setUpWindow();
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
	QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
	QMenu* trayIconMenu = new QMenu;
	trayIconMenu->setObjectName(kTrayMenu);
	trayIconMenu->setStyleSheet(m_menuQss);

	// start up
	QAction* startUpAction = new QAction(trayIconMenu);
	startUpAction->setText(tr("Start up"));
	startUpAction->setCheckable(true);
	startUpAction->setObjectName(kStartUpAction);

	// history size
	NumMenuActionWidget* historySizeWidget = new NumMenuActionWidget(tr("History Size"),
		kHistorySizeMin, m_historySize, kHistroySizeMax, trayIconMenu);
	QWidgetAction* historySizeAction = new QWidgetAction(trayIconMenu);
	historySizeAction->setDefaultWidget(historySizeWidget);

	// number of tips windows
	NumMenuActionWidget* tipsNumWidget = new NumMenuActionWidget(tr("Num of tips"),
		kTipsNumMin, m_tipsWindowState.size(), kTipsNumMax, trayIconMenu);
	QWidgetAction* tipsNumAction = new QWidgetAction(trayIconMenu);
	tipsNumAction->setDefaultWidget(tipsNumWidget);

	// exit
	QAction* exitAction = new QAction(trayIconMenu);
	exitAction->setText(tr("Exit"));

	trayIconMenu->addAction(startUpAction);
	trayIconMenu->addAction(historySizeAction);
	trayIconMenu->addAction(tipsNumAction);
	trayIconMenu->addAction(exitAction);

	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/res/image/startup.png"));	// neccessary
	trayIcon->show();
	trayIcon->showMessage("ClipboardEx", tr("is enabled"), trayIcon->icon(), kTrayMsgShowTime);

	connect(startUpAction, &QAction::triggered, this, [](bool b) {
		RegeditManager::getInstance()->enableRunStartUp(b);
	});
	connect(tipsNumWidget, &NumMenuActionWidget::sigNumChange, this, &MainControl::onTipsWindowNumChange);
	connect(historySizeWidget, &NumMenuActionWidget::sigNumChange, this, [this](int n) {
		m_historySize = n; 
		HistoryDataList::getInstance()->onSetListSize(m_historySize); 
	});
	connect(exitAction, &QAction::triggered, this, &MainControl::onSaveConfigure);
	connect(exitAction, &QAction::triggered, this, &QApplication::quit);
	connect(trayIcon, &QSystemTrayIcon::activated, this, &MainControl::onTrayActivated);
}

void MainControl::setUpQss()
{
	QLocale local;
	bool isChinese = local.language() == QLocale::Chinese;
	QString menuName = isChinese ? ":/qss/res/qss/menu_cn.qss" : ":/qss/res/qss/menu.qss";
	QString winName = isChinese ? ":/qss/res/qss/window_cn.qss" : ":/qss/res/qss/window.qss";
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
}

void MainControl::onTrayActivated(QSystemTrayIcon::ActivationReason reson)
{
	if (reson != QSystemTrayIcon::DoubleClick) return;
	for (auto& tips : m_tipsWindows)
		tips->show();
}
