#include "ClipboardTipsWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QList>

HistoryDataList* HistoryDataList::getInstance() 
{
	static HistoryDataList instance;
	return &instance;
}

HistoryDataList::HistoryDataList(QObject* parent = nullptr) : QObject(parent)
{
	connect(QApplication::clipboard(), &QClipboard::dataChanged, getInstance(), &HistoryDataList::onClipboardDataUpdate);
}

HistoryDataList::~HistoryDataList()
{
}

void HistoryDataList::onSetListSize(int s)
{
	if (s < m_historyClipboardDataList.size()) {
		while (m_historyClipboardDataList.size() > s) m_historyClipboardDataList.pop_front();
		emit sigDataListUpdate();
	}
	m_listSize = s;
}

void HistoryDataList::onClipboardDataUpdate()
{
	const QMimeData* data = QApplication::clipboard()->mimeData();
	if (m_historyClipboardDataList.size() >= m_listSize) {
		m_historyClipboardDataList.pop_front();
	}
	m_historyClipboardDataList.push_back(data);
	emit sigDataListUpdate();
}

ClipboardTipsWindow::ClipboardTipsWindow(QWidget* parent /*= nullptr*/)
	: DockableWindow(parent)
{

}

void ClipboardTipsWindow::initWindow()
{

}