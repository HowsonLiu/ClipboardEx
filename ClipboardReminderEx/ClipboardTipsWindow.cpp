#include "ClipboardTipsWindow.h"
#include "HistoryDataList.h"
#include "def.h"
#include <QList>
#include <QListWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPropertyAnimation>
#include <QMimeData>
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QClipboard>

namespace {
	static float g_expandSpeed = 1;
}

MimeDataLabel::MimeDataLabel(QWidget* parent /*= nullptr*/) : QLabel(parent)
	, m_bindMimeData(nullptr)
{
}

void MimeDataLabel::showMimeData()
{
	if (!m_bindMimeData) return;

	if (m_bindMimeData->hasImage()) {
		QSize originSize = m_bindMimeData->image.size();
		setPixmap(QPixmap::fromImage(m_bindMimeData->image).scaled(size(), Qt::KeepAspectRatio));
		return;
	}

	QString text;
	if (m_bindMimeData->hasText()) {
		text = m_bindMimeData->text;
	}
	else if (m_bindMimeData->hasUrls()) {
		for(auto url : m_bindMimeData->urls)
			text += url.toString() + "\n";
	}
	else {
		text = "UnKnown";
	}
	setWordWrap(!text.contains('\n'));
	setText(text);
	// setPixmap(QPixmap::fromImage(qvariant_cast<QImage>(QApplication::clipboard()->mimeData()->imageData())));
}

//void MimeDataLabel::onUpdateSize(const QSize& size)
//{
//	setFixedSize(size);
//	showMimeData();		// update image size
//}

void MimeDataLabel::onDoubleClicked()
{
	if (!m_bindMimeData || !m_bindMimeData->isValid()) return;
	m_bindMimeData->copyToClipboard();
}

ClipboardTipsWindowState::ClipboardTipsWindowState() : bExpand(false), 
	bAutoShow(true), dockState()
{
}

ClipboardTipsWindowState::ClipboardTipsWindowState(const QString& str) : bExpand(false),
	bAutoShow(true), dockState()
{
	if (str.isEmpty()) return;
	QStringList list = str.split('/');
	if (list.size() < 3) return;
	bExpand = list[0].toInt();
	bAutoShow = list[1].toInt();
	dockState = list[2];
}

ClipboardTipsWindowState::operator QString()
{
	return QString::number(bExpand) + '/' + QString::number(bAutoShow) + '/' + dockState;
}

ClipboardTipsWindow::ClipboardTipsWindow(QWidget* parent /*= nullptr*/)
	: DockableWindow(parent)
{
	initWindow();
	connect(m_historyMimeDataListWidget, &QListWidget::itemDoubleClicked, this, &ClipboardTipsWindow::onItemDoubleClicked);
	//connect(this, &ClipboardTipsWindow::sigUpdateLabelSize, m_curMimeDataLabel, &MimeDataLabel::onUpdateSize);
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
	m_historyMimeDataListWidget->setVisible(state.bExpand);
	m_autoShowCheckBox->setChecked(state.bAutoShow);
	loadDockableState(state.dockState);
}

void ClipboardTipsWindow::updateHistoryList()
{
	auto dataList = HistoryDataList::getInstance()->dataList();
	// m_curMimeDataLabel->setMimeData(dataList->at(0));
	while (m_historyMimeDataListWidget->count() < dataList->size() - 1) {
		QListWidgetItem* item = new QListWidgetItem(m_historyMimeDataListWidget);
		MimeDataLabel* label = new MimeDataLabel(this);
		//connect(this, &ClipboardTipsWindow::sigUpdateLabelSize, label, &MimeDataLabel::onUpdateSize);
		//connect(this, &ClipboardTipsWindow::sigUpdateLabelSize, [item](const QSize& size) {
		//	item->setSizeHint(QSize(item->sizeHint().width(), size.height()));
		//});
		m_historyMimeDataListWidget->addItem(item);
		m_historyMimeDataListWidget->setItemWidget(item, label);
		emit sigUpdateLabelSize(m_curMimeDataLabel->size());
	}
	while (m_historyMimeDataListWidget->count() > dataList->size() - 1) {
		auto item = m_historyMimeDataListWidget->takeItem(0);
		m_historyMimeDataListWidget->removeItemWidget(item);
	}
	for (int i = 0; i < dataList->size() - 1; i++) {
		auto item = m_historyMimeDataListWidget->item(i);
		MimeDataLabel* label = dynamic_cast<MimeDataLabel*>(m_historyMimeDataListWidget->itemWidget(item));
		label->setMimeData(dataList->at(i + 1));
	}
}

void ClipboardTipsWindow::setLabelSize(const QSize& size)
{
	emit sigUpdateLabelSize(size);
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

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_curMimeDataLabel);
	layout->addWidget(m_historyMimeDataListWidget);
	layout->addWidget(m_expandCheckBox, 0, Qt::AlignHCenter);
	layout->addWidget(m_autoShowCheckBox, 0, Qt::AlignRight);
	setLayout(layout);

	m_curMimeDataLabel->setObjectName(kMainLabel);
	m_historyMimeDataListWidget->setObjectName(kLabelList);
	m_expandCheckBox->setObjectName(kExpandBtn);
	m_autoShowCheckBox->setObjectName(kAutoShowBtn);

	m_autoShowCheckBox->setText(tr("Auto show"));
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::ToolTip);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

void ClipboardTipsWindow::onHistoryListUpdate()
{
	updateHistoryList();
}

void ClipboardTipsWindow::onExpandStateChanged(int state)
{
	// m_historyMimeDataListWidget->setFixedHeight(m_listHeight);
	m_historyMimeDataListWidget->setVisible(Qt::CheckState::Unchecked != state);
	adjustSize();
}

void ClipboardTipsWindow::onItemDoubleClicked(QListWidgetItem* item)
{
	MimeDataLabel* label = dynamic_cast<MimeDataLabel*>(m_historyMimeDataListWidget->itemWidget(item));
	if (!label) return;
	label->onDoubleClicked();
}
