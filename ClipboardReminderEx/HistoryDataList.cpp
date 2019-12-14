#include "HistoryDataList.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QTime>
#include <QTimer>
#include <QDebug>

namespace {
	static const int g_readMaxTime = 5 * 1000;
}

void ClipboardData::clear()
{
	image = QImage();
	text.clear();
	urls.clear();
}

void ClipboardData::copyFromClipboard()
{
	const QMimeData* mimeData = QApplication::clipboard()->mimeData();
	if (mimeData->hasImage()) image = qvariant_cast<QImage>(mimeData->imageData()).copy();
	else if (mimeData->hasText()) text = mimeData->text();
	else if (mimeData->hasHtml()) text = mimeData->html();
	else if (mimeData->hasUrls()) urls = mimeData->urls();
}

void ClipboardData::copyToClipboard()
{
	if (!isValid()) return;
	auto clipboard = QApplication::clipboard();
	if (!image.isNull()) clipboard->setImage(image, QClipboard::Clipboard);
	else if (!text.isEmpty()) clipboard->setText(text, QClipboard::Clipboard);
	else if (!urls.isEmpty()) clipboard->setText(urls[0].toString(), QClipboard::Clipboard);
}

void ReadClipboardThread::run()
{
	QTime startTime = QTime::currentTime();
	m_data.clear();
	while (startTime.msecsTo(QTime::currentTime()) < g_readMaxTime  && !m_data.isValid()) {
		m_data.copyFromClipboard();
		this->sleep(0);
	}
	if (m_data.isValid()) {
		emit sigSuccessed(m_data);
		qDebug() << "success";
	}
	else {
		emit sigFailed();
		qDebug() << "failed";
	}
}

HistoryDataList* HistoryDataList::getInstance()
{
	static HistoryDataList instance;
	return &instance;
}

HistoryDataList::HistoryDataList(QObject* parent) : QObject(parent)
{
	m_thread = new ReadClipboardThread(this);
	// m_historyClipboardDataList.push_front(std::make_shared<ClipboardData>(QApplication::clipboard()->mimeData()));
	connect(m_thread, &ReadClipboardThread::sigSuccessed, this, &HistoryDataList::onClipboardDataUpdate);
	connect(QApplication::clipboard(), &QClipboard::dataChanged, [this]() {m_thread->start(); });
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

void HistoryDataList::onClipboardDataUpdate(const ClipboardData& data)
{
	while (m_historyClipboardDataList.size() >= m_listSize)
		m_historyClipboardDataList.pop_back();
	// m_historyClipboardDataList.push_front(std::make_shared<ClipboardData>(QApplication::clipboard()->mimeData()));
	emit sigDataListUpdate();
}