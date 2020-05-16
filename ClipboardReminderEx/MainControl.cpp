#include "MainControl.h"
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"
#include "ConfigManager.h"
#include "HistoryDataList.h"
#include "def.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QWidgetAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

namespace {
	static const QString g_historySizeDescribeText = QObject::tr("History Size");
	static const int g_historySizeMin = 1;
	static const int g_historySizeDefault = 5;
	static const int g_histroySizeMax = 20;
	static const QString g_tipsNumDescribeText = QObject::tr("Num of tips");
	static const int g_tipsNumMin = 1;
	static const int g_tipsNumDefault = 1;
	static const int g_tipsNumMax = 20;
	static const QString g_tipsRectDescribeText = "Label";
	static const QString g_tipsRectHeightText = "Height";
	static const QString g_tipsRectWidthText = "Width ";
	static const int g_tipsRectHeightMin = 108;
	static const int g_tipsRectHeightDefault = 216;
	static const int g_tipsRectHeightMax = 432;
	static const int g_tipsRectWidthMin = 192;
	static const int g_tipsRectWidthDefault = 384;
	static const int g_tipsRectWidthMax = 768;
	static const QString g_tipsListHeightText = "Height of list";
	static const int g_tipsListHeightMin = 300;
	static const int g_tipsListHeightDefault = 500;
	static const int g_tipsListHeightMax = 800;
	static const int g_autoSaveInterval = 5 * 1000;

	const QString kThemeQss = "theme.qss";
}

MainControl::MainControl(QObject* parent /*= nullptr*/) : QObject(parent)
	, m_tipsWindowState()
	, m_historySize(g_historySizeDefault)
{
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
		window->setStyleSheet(m_windowQss);
		window->updateHistoryList();
		window->show();
		window->move(QApplication::desktop()->screen()->rect().center() - window->rect().center());
		m_tipsWindows.push_back(window);
	}
	else {
		for (auto state : m_tipsWindowState) {
			ClipboardTipsWindow* window = new ClipboardTipsWindow;
			window->setStyleSheet(m_windowQss);
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
	NumMenuActionWidget* historySizeWidget = new NumMenuActionWidget(g_historySizeDescribeText,
		g_historySizeMin, m_historySize, g_histroySizeMax, trayIconMenu);
	QWidgetAction* historySizeAction = new QWidgetAction(trayIconMenu);
	historySizeAction->setDefaultWidget(historySizeWidget);

	// number of tips windows
	NumMenuActionWidget* tipsNumWidget = new NumMenuActionWidget(g_tipsNumDescribeText,
		g_tipsNumMin, m_tipsWindowState.size(), g_tipsNumMax, trayIconMenu);
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

	connect(startUpAction, &QAction::triggered, this, [](bool b) {
		RegeditManager::getInstance()->enableRunStartUp(b);
	});
	connect(tipsNumWidget, &NumMenuActionWidget::sigNumChange, this, &MainControl::onTipsWindowNumChange);
	connect(historySizeWidget, &NumMenuActionWidget::sigNumChange, this, [this](int n) {
		m_historySize = n; 
		HistoryDataList::getInstance()->onSetListSize(m_historySize); 
	});
	connect(exitAction, &QAction::triggered, this, &QApplication::quit);
	connect(trayIcon, &QSystemTrayIcon::activated, this, [=]() {	trayIconMenu->adjustSize(); });
}

void MainControl::setUpQss()
{
	do {
		QFile qssFile;
		qssFile.setFileName(":/res/qss/menu.qss");
		if (!qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) break;
		m_menuQss = qssFile.readAll();
		qssFile.close();

		qssFile.setFileName(":/res/qss/window.qss");
		if (!qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) break;
		m_windowQss = qssFile.readAll();
		qssFile.close();
	} while (false);
}

void MainControl::setAutoSave()
{
	QTimer* timer = new QTimer(this);
	timer->setSingleShot(false);
	timer->start(g_autoSaveInterval);
	connect(timer, &QTimer::timeout, this, &MainControl::onSaveConfigure);
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
		window->move(QApplication::desktop()->screen()->rect().center() - window->rect().center());
		window->show();
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
