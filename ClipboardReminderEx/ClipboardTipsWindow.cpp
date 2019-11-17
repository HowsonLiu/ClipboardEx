#include "ClipboardTipsWindow.h"
#include "HistoryDataList.h"
#include <QList>
#include <QListWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPropertyAnimation>
#include <QMimeData>
#include <QUrl>
#include <QDebug>

namespace {
	static float g_expandSpeed = 1;
}

MimeDataLabel::MimeDataLabel(QWidget* parent /*= nullptr*/) : QLabel(parent)
	, m_bDisplay(true), m_bindMimeData(nullptr)
{
	setWordWrap(true);
	setStyleSheet("background-color:yellow");
}

void MimeDataLabel::showMimeData()
{
	if (!m_bindMimeData) return;
	if (m_bindMimeData->hasImage()) {
		setPixmap(QPixmap::fromImage(qvariant_cast<QImage>(m_bindMimeData->imageData()).scaled(width(), height(), Qt::KeepAspectRatio)));
		setAlignment(Qt::AlignCenter);
		return;
	}
	QString content;
	if (m_bindMimeData->hasText())
		content = m_bindMimeData->text();
	else if (m_bindMimeData->hasHtml()) {
		content = m_bindMimeData->html();
	}
	else if (m_bindMimeData->hasUrls()) {
		foreach(QUrl url, m_bindMimeData->urls()) {
			content += url.toString() + "\n";
		}
	}
	if (content.isEmpty()) content = "Unknow";
	setText(content);
}

void MimeDataLabel::onUpdateSize(const QSize& size)
{
	setFixedSize(size);
	showMimeData();		// update image size
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
	beautyWindow();
	connect(HistoryDataList::getInstance(), &HistoryDataList::sigDataListUpdate, this, &ClipboardTipsWindow::onHistoryListUpdate);
	connect(m_expandCheckBox, &QCheckBox::stateChanged, this, &ClipboardTipsWindow::onExpandStateChanged);
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

void ClipboardTipsWindow::updateHistoryList()
{
	auto dataList = HistoryDataList::getInstance()->dataList();
	m_curMimeDataLabel->setMimeData(dataList->back());
	while (m_historyMimeDataListWidget->count() < dataList->size() - 1) {
		QListWidgetItem* item = new QListWidgetItem(m_historyMimeDataListWidget);
		MimeDataLabel* label = new MimeDataLabel(this);
		connect(this, &ClipboardTipsWindow::sigUpdateLabelSize, label, &MimeDataLabel::onUpdateSize);
		connect(this, &ClipboardTipsWindow::sigUpdateLabelSize, [item](const QSize& size) {
			item->setSizeHint(QSize(item->sizeHint().width(), size.height()));
		});
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
		label->setMimeData(dataList->at(dataList->size() - 2 - i));
	}
}

void ClipboardTipsWindow::setLabelSize(const QSize& size)
{
	emit sigUpdateLabelSize(size);
	m_curMimeDataLabel->onUpdateSize(size);
	m_historyMimeDataListWidget->setFixedWidth(size.width());
	adjustSize();
}

void ClipboardTipsWindow::setListHeight(int h)
{
	m_listHeight = h;
	if (m_expandCheckBox->isChecked()) {
		m_historyMimeDataListWidget->setFixedHeight(m_listHeight);
		adjustSize();
	}
}

void ClipboardTipsWindow::initWindow()
{
	m_curMimeDataLabel = new MimeDataLabel(this);
	m_historyMimeDataListWidget = new QListWidget(this);
	m_expandCheckBox = new QCheckBox(this);
	m_autoShowCheckBox = new QCheckBox(this);
	m_expandAnimation = new QPropertyAnimation(m_historyMimeDataListWidget);
	m_shrinkAnimation = new QPropertyAnimation(m_historyMimeDataListWidget);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_curMimeDataLabel);
	layout->addWidget(m_historyMimeDataListWidget);
	layout->addWidget(m_expandCheckBox);
	QHBoxLayout* bottomLayout = new QHBoxLayout(this);
	bottomLayout->addStretch();
	bottomLayout->addWidget(m_autoShowCheckBox);
	layout->addLayout(bottomLayout);
	setLayout(layout);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void ClipboardTipsWindow::beautyWindow()
{
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::ToolTip);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

void ClipboardTipsWindow::onHistoryListUpdate()
{
	updateHistoryList();
}

void ClipboardTipsWindow::onExpandStateChanged(int state)
{
	m_expandAnimation->stop();
	m_shrinkAnimation->stop();
	if (Qt::CheckState::Unchecked == state) {
		m_historyMimeDataListWidget->setMinimumSize(0, 0);
		m_shrinkAnimation->setStartValue(m_historyMimeDataListWidget->size());
		m_shrinkAnimation->setEndValue(QSize(m_historyMimeDataListWidget->width(), 0));
		m_shrinkAnimation->setDuration(m_listHeight * g_expandSpeed);
		m_shrinkAnimation->start();
	}
	else {
		m_historyMimeDataListWidget->setMinimumSize(0, 0);
		m_expandAnimation->setStartValue(m_historyMimeDataListWidget->size());
		m_expandAnimation->setEndValue(QSize(m_historyMimeDataListWidget->width(), m_listHeight));
		m_expandAnimation->setDuration(m_listHeight * g_expandSpeed);
		m_expandAnimation->start();
	}
}