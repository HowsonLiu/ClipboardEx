#include "HistoryDataList.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>

HistoryDataList* HistoryDataList::getInstance()
{
	static HistoryDataList instance;
	return &instance;
}

HistoryDataList::HistoryDataList(QObject* parent) : QObject(parent)
{
	m_historyClipboardDataList.push_back(deepCopyMimeData(QApplication::clipboard()->mimeData()));
	connect(QApplication::clipboard(), &QClipboard::dataChanged, getInstance(), &HistoryDataList::onClipboardDataUpdate);
}

HistoryDataList::~HistoryDataList()
{
}

void HistoryDataList::onSetListSize(int s)
{
	if (s < m_historyClipboardDataList.size()) {
		while (m_historyClipboardDataList.size() > s) {
			QMimeData* data = m_historyClipboardDataList.front();
			m_historyClipboardDataList.pop_front();
			delete data;
		}
		emit sigDataListUpdate();
	}
	m_listSize = s;
}

void HistoryDataList::onClipboardDataUpdate()
{
	while (m_historyClipboardDataList.size() >= m_listSize) {
		QMimeData* data = m_historyClipboardDataList.front();
		m_historyClipboardDataList.pop_front();
		delete data;
	}
	m_historyClipboardDataList.push_back(deepCopyMimeData(QApplication::clipboard()->mimeData()));
	emit sigDataListUpdate();
}

QMimeData* deepCopyMimeData(const QMimeData* src)
{
	QMimeData* dst = new QMimeData;
	// Only copy the data which we can see
	if (src->hasImage())
		dst->setImageData(src->imageData());
	else if (src->hasText()) dst->setText(src->text());
	else if (src->hasUrls()) dst->setUrls(src->urls());
	else if (src->hasHtml()) dst->setHtml(src->html());
	return dst;
}