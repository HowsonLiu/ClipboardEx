#include "HistoryDataList.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>

ClipboardData::ClipboardData(const QMimeData* mimedata)
{
	if (mimedata->hasImage()) image = qvariant_cast<QImage>(mimedata->imageData());
	else if (mimedata->hasText()) text = mimedata->text();
	else if (mimedata->hasHtml()) text = mimedata->html();
	else if (mimedata->hasUrls()) urls = mimedata->urls();
}

HistoryDataList* HistoryDataList::getInstance()
{
	static HistoryDataList instance;
	return &instance;
}

HistoryDataList::HistoryDataList(QObject* parent) : QObject(parent)
{
	m_historyClipboardDataList.push_front(std::make_shared<ClipboardData>(QApplication::clipboard()->mimeData()));
	connect(QApplication::clipboard(), &QClipboard::dataChanged, getInstance(), &HistoryDataList::onClipboardDataUpdate);
}

HistoryDataList::~HistoryDataList()
{
}

void HistoryDataList::onSetListSize(int s)
{
	if (s < m_historyClipboardDataList.size()) {
		while (m_historyClipboardDataList.size() > s)
			m_historyClipboardDataList.pop_back();
		emit sigDataListUpdate();
	}
	m_listSize = s;
}

void HistoryDataList::onClipboardDataUpdate()
{
	while (m_historyClipboardDataList.size() >= m_listSize)
		m_historyClipboardDataList.pop_back();
	m_historyClipboardDataList.push_front(std::make_shared<ClipboardData>(QApplication::clipboard()->mimeData()));
	emit sigDataListUpdate();
}