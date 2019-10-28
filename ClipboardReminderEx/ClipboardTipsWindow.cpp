#include "ClipboardTipsWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QList>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMimeData>

HistoryDataList* HistoryDataList::getInstance() 
{
	static HistoryDataList instance;
	return &instance;
}

HistoryDataList::HistoryDataList(QObject* parent) : QObject(parent)
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

MimeDataLabel::MimeDataLabel(QWidget* parent /*= nullptr*/)
{

}


void MimeDataLabel::showMimeData(const QMimeData* data)
{
	if (data->hasImage()) {
		QImage img = qvariant_cast<QImage>(data->imageData());
		setPixmap(QPixmap::fromImage(img));
	}
	else if (data->hasText() || data->hasUrls() || data->hasHtml())
		setText(data->text());
	else
		setText("Unknow");
}

ClipboardTipsWindow::ClipboardTipsWindow(QWidget* parent /*= nullptr*/)
	: DockableWindow(parent)
{
	initWindow();
	m_curMimeDataLabel->showMimeData(QApplication::clipboard()->mimeData());
}

void ClipboardTipsWindow::initWindow()
{
	m_curMimeDataLabel = new MimeDataLabel(this);
	m_historyMimeDataListWidget = new QListWidget(this);
	m_expandButton = new QPushButton(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_curMimeDataLabel);
	layout->addWidget(m_historyMimeDataListWidget);
	layout->addWidget(m_expandButton);

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::ToolTip);
}