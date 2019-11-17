#include "MainControl.h"
#include "MenuActionWidget.h"
#include "ClipboardTipsWindow.h"
#include "ConfigManager.h"
#include "HistoryDataList.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QWidgetAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

namespace {
	static const QString g_historySizeDescribeText = "History Size";
	static const int g_historySizeMin = 1;
	static const int g_historySizeDefault = 5;
	static const int g_histroySizeMax = 20;
	static const QString g_tipsNumDescribeText = "Num of tips";
	static const int g_tipsNumMin = 1;
	static const int g_tipsNumDefault = 1;
	static const int g_tipsNumMax = 20;
	static const QString g_tipsRectDescribeText = "Label";
	static const QString g_tipsRectHeightText = "Height";
	static const QString g_tipsRectWidthText = "Width";
	static const int g_tipsRectHeightMin = 300;
	static const int g_tipsRectHeightDefault = 400;
	static const int g_tipsRectHeightMax = 500;
	static const int g_tipsRectWidthMin = 300;
	static const int g_tipsRectWidthDefault = 400;
	static const int g_tipsRectWidthMax = 500;
	static const QString g_tipsListHeightText = "Height of list";
	static const int g_tipsListHeightMin = 300;
	static const int g_tipsListHeightDefault = 500;
	static const int g_tipsListHeightMax = 800;
	static const int g_autoSaveInterval = 5 * 1000;
}

MainControl::MainControl(QObject* parent /*= nullptr*/) : QObject(parent)
	, m_tipsWindowState()
	, m_historySize(g_historySizeDefault), m_tipsListHeight(g_tipsListHeightDefault)
	, m_tipsRectSize({g_tipsRectWidthDefault, g_tipsRectHeightDefault})
{
}

void MainControl::readConfig()
{
	auto tipsWindowState = IniManager::getInstance()->getWindowPositions();
	int historySize = IniManager::getInstance()->getHistorySize();
	int tipsListHeight = IniManager::getInstance()->getTipsListHeight();
	auto tipsRectSize = IniManager::getInstance()->getTipsRectSize();

	if (historySize) m_historySize = historySize;
	if (tipsListHeight) m_tipsListHeight = tipsListHeight;
	if (tipsRectSize.isValid()) m_tipsRectSize = tipsRectSize;
	if (!tipsWindowState.isEmpty()) m_tipsWindowState = tipsWindowState;
}

void MainControl::setUpUI()
{
	setUpTrayIcon();
	HistoryDataList::getInstance()->onSetListSize(m_historySize);
	if (m_tipsWindowState.isEmpty()) {
		ClipboardTipsWindow* window = new ClipboardTipsWindow;
		window->updateHistoryList();
		window->setLabelSize(m_tipsRectSize);
		window->setListHeight(m_tipsListHeight);
		window->show();
		window->move(QApplication::desktop()->screen()->rect().center() - window->rect().center());
		m_tipsWindows.push_back(window);
	}
	else {
		for (auto state : m_tipsWindowState) {
			ClipboardTipsWindow* window = new ClipboardTipsWindow;
			window->updateHistoryList();
			window->setLabelSize(m_tipsRectSize);
			window->setListHeight(m_tipsListHeight);
			window->show();
			window->loadTipsWindowState(state);
			m_tipsWindows.push_back(window);
		}
	}
}

void MainControl::setUpTrayIcon()
{
	QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
	QMenu* trayIconMenu = new QMenu;
	NumMenuActionWidget* historySizeWidget = new NumMenuActionWidget(g_historySizeDescribeText,
		g_historySizeMin, m_historySize, g_histroySizeMax, trayIconMenu);
	QWidgetAction* historySizeAction = new QWidgetAction(trayIconMenu);
	NumMenuActionWidget* tipsNumWidget = new NumMenuActionWidget(g_tipsNumDescribeText,
		g_tipsNumMin, m_tipsWindowState.size(), g_tipsNumMax, trayIconMenu);
	QWidgetAction* tipsNumAction = new QWidgetAction(trayIconMenu);
	DoubleSliderMenuActionWidget* tipsRectWidget = new DoubleSliderMenuActionWidget(g_tipsRectDescribeText,
		g_tipsRectWidthText, g_tipsRectWidthMin, m_tipsRectSize.width(), g_tipsRectWidthMax,
		g_tipsRectHeightText, g_tipsRectHeightMin, m_tipsRectSize.height(), g_tipsRectHeightMax, trayIconMenu);
	QWidgetAction* tipsRectAction = new QWidgetAction(trayIconMenu);
	SliderMenuActionWidget* tipsListHeightWidget = new SliderMenuActionWidget(g_tipsListHeightText,
		g_tipsListHeightMin, m_tipsListHeight, g_tipsListHeightMax, trayIconMenu);
	QWidgetAction* tipsListAction = new QWidgetAction(trayIconMenu);

	historySizeAction->setDefaultWidget(historySizeWidget);
	trayIconMenu->addAction(historySizeAction);
	tipsNumAction->setDefaultWidget(tipsNumWidget);
	trayIconMenu->addAction(tipsNumAction);
	tipsRectAction->setDefaultWidget(tipsRectWidget);
	trayIconMenu->addAction(tipsRectAction);
	tipsListAction->setDefaultWidget(tipsListHeightWidget);
	trayIconMenu->addAction(tipsListAction);
	trayIconMenu->addAction("Exit");

	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon("c:\\Users\\liuhaosheng\\Pictures\\1.png"));	// neccessary
	trayIcon->show();

	connect(tipsNumWidget, &NumMenuActionWidget::sigNumChange, this, &MainControl::onTipsWindowNumChange);
	connect(historySizeWidget, &NumMenuActionWidget::sigNumChange, [this](int n) {
		m_historySize = n; 
		HistoryDataList::getInstance()->onSetListSize(m_historySize); 
	});
	connect(tipsListHeightWidget, &SliderMenuActionWidget::sigNumChange, [this](int n) {
		m_tipsListHeight = n; 
		for (auto tip : m_tipsWindows) tip->setListHeight(m_tipsListHeight);
	});
	connect(tipsRectWidget, &DoubleSliderMenuActionWidget::sigNumChange1, [this](int n) {
		m_tipsRectSize.setWidth(n);
		for(auto tip : m_tipsWindows) tip->setLabelSize(m_tipsRectSize);
	});
	connect(tipsRectWidget, &DoubleSliderMenuActionWidget::sigNumChange2, [this](int n) {
		m_tipsRectSize.setHeight(n);
		for (auto tip : m_tipsWindows) tip->setLabelSize(m_tipsRectSize);
	});
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
	IniManager::getInstance()->setTipsListHeight(m_tipsListHeight);
	IniManager::getInstance()->setTipsRectSize(m_tipsRectSize);
}
