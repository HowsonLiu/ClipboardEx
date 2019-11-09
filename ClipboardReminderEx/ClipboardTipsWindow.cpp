#include "ClipboardTipsWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QList>
#include <QListWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMimeData>
#include <QPropertyAnimation>

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
	setWordWrap(true);
	setMaximumSize(300, 300);
	setScaledContents(true);
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

void MimeDataLabel::resizeEvent(QResizeEvent* event)
{
	QPixmap* p = const_cast<QPixmap*>(pixmap());
	if (p) setPixmap(p->scaled(width(), height(), Qt::KeepAspectRatio));
}

ClipboardTipsWindowState::ClipboardTipsWindowState() : bExpand(false), 
	bAutoShow(true), dockState()
{
}

ClipboardTipsWindowState::ClipboardTipsWindowState(const QString& str) : bExpand(false),
	bAutoShow(true), dockState()
{
	if (str.isEmpty()) return;
	QStringList list = str.split('-');
	if (list.size() < 3) return;
	bExpand = list[0].toInt();
	bAutoShow = list[1].toInt();
	dockState = list[2];
}

ClipboardTipsWindowState::operator QString()
{
	return QString::number(bExpand) + '-' + QString::number(bAutoShow) + '-' + dockState;
}

ClipboardTipsWindow::ClipboardTipsWindow(QWidget* parent /*= nullptr*/)
	: DockableWindow(parent)
{
	initWindow();
	connect(HistoryDataList::getInstance(), &HistoryDataList::sigDataListUpdate, this, &ClipboardTipsWindow::onHistoryListUpdate);
}

ClipboardTipsWindowState ClipboardTipsWindow::getTipsWindowState() const
{
	ClipboardTipsWindowState res;
	res.bExpand = m_expandCheckBox->isChecked();
	res.bAutoShow = m_autoShowCheckBox->isChecked();
	res.dockState = getDockableState();
	return res;
}

void ClipboardTipsWindow::loadTipsWindowState(const ClipboardTipsWindowState& state)
{
	m_expandCheckBox->setChecked(state.bExpand);
	m_autoShowCheckBox->setChecked(state.bAutoShow);
	loadDockableState(state.dockState);
}

void ClipboardTipsWindow::initWindow()
{
	m_curMimeDataLabel = new MimeDataLabel(this);
	m_historyMimeDataListWidget = new QListWidget(this);
	m_expandCheckBox = new QCheckBox(this);
	m_autoShowCheckBox = new QCheckBox(this);
	m_expandAnimation = new QPropertyAnimation(this);
	m_shrinkAnimation = new QPropertyAnimation(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_curMimeDataLabel);
	layout->addWidget(m_historyMimeDataListWidget);
	layout->addWidget(m_expandCheckBox);
	QHBoxLayout* bottomLayout = new QHBoxLayout(this);
	bottomLayout->addStretch();
	bottomLayout->addWidget(m_autoShowCheckBox);
	layout->addLayout(bottomLayout);
	setLayout(layout);

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::ToolTip);
}

void ClipboardTipsWindow::onHistoryListUpdate()
{
	auto dataList = HistoryDataList::getInstance()->dataList();
	m_curMimeDataLabel->showMimeData(dataList->back());
	while (m_historyMimeDataListWidget->count() < dataList->size() - 1) {
		QListWidgetItem* item = new QListWidgetItem(m_historyMimeDataListWidget);
		MimeDataLabel* label = new MimeDataLabel(this);
		m_historyMimeDataListWidget->addItem(item);
		m_historyMimeDataListWidget->setItemWidget(item, label);
	}
	while (m_historyMimeDataListWidget->count() > dataList->size() - 1) {
		auto item = m_historyMimeDataListWidget->takeItem(0);
		m_historyMimeDataListWidget->removeItemWidget(item);
	}
	for (int i = 0; i < dataList->size() - 1; i++) {
		auto item = m_historyMimeDataListWidget->item(i);
		MimeDataLabel* label = dynamic_cast<MimeDataLabel*>(m_historyMimeDataListWidget->itemWidget(item));
		label->showMimeData(dataList->at(dataList->size() - 2 - i));
	}
}

void ClipboardTipsWindow::onCheckBoxStateChanged(int state)
{
	if (Qt::CheckState::Unchecked == state) {

	}
	else {

	}
}