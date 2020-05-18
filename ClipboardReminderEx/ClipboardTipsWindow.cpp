#include "ClipboardTipsWindow.h"
#include "HistoryDataList.h"
#include "def.h"
#include "util.h"
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
#include <QTimer>
#include <QPainter>
#include <QScrollBar>

namespace {
	const float kExpandSpeed = 1;
	const float kShowTime = 3.0f;
}

MimeDataLabel::MimeDataLabel(QWidget* parent /*= nullptr*/) : QLabel(parent)
	, m_bindMimeData(nullptr)
{
}

void MimeDataLabel::showMimeData()
{
	if (!m_bindMimeData) return;

	if (m_bindMimeData->hasImage()) {
		setAlignment(Qt::AlignCenter);
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
	showText(text);
}

void MimeDataLabel::showText(const QString& text)
{
	setWordWrap(!text.contains('\n'));
	setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	int fontSize = kMinFontSize;
	QFont f = font();
	f.setPixelSize(fontSize);
	QFontMetrics fm(f);
	QRect rec = fm.boundingRect(text);
	while (rec.width() < this->width() && rec.height() < this->height()) {
		f.setPixelSize(++fontSize);
		fm = QFontMetrics(f);
		rec = fm.boundingRect(text);
	}
	f.setPixelSize(fontSize - 1);
	setFont(f);
	setText(text);
}

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
	connect(HistoryDataList::getInstance(), &HistoryDataList::sigDataListUpdate, this, &ClipboardTipsWindow::onHistoryListUpdate);
	connect(m_expandCheckBox, &QCheckBox::stateChanged, this, &ClipboardTipsWindow::onExpandStateChanged);
	connect(m_timer, &QTimer::timeout, this, &ClipboardTipsWindow::hide);
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
	m_curMimeDataLabel->setMimeData(dataList->at(0));
	while (m_historyMimeDataListWidget->count() < dataList->size() - 1) {
		QListWidgetItem* item = new QListWidgetItem(m_historyMimeDataListWidget);
		MimeDataLabel* label = new MimeDataLabel(this);
		label->setObjectName(kSubLabel);
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
		label->setMimeData(dataList->at(i + 1));
	}
}

void ClipboardTipsWindow::show()
{
	static bool first = true;
	if (first) {
		m_historyMimeDataListWidget->verticalScrollBar()->setStyleSheet(this->styleSheet());
		first = false;
	}
	if (m_autoShowCheckBox->isChecked()) {
		m_timer->start(kShowTime * 1000);
	}
	__super::show();
}

void ClipboardTipsWindow::initWindow()
{
	m_curMimeDataLabel = new MimeDataLabel(this);
	m_historyMimeDataListWidget = new QListWidget(this);
	m_expandCheckBox = new QCheckBox(this);
	m_autoShowCheckBox = new QCheckBox(this);
	m_timer = new QTimer(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_curMimeDataLabel);
	layout->addWidget(m_historyMimeDataListWidget);
	layout->addWidget(m_expandCheckBox, 0, Qt::AlignHCenter);
	layout->addWidget(m_autoShowCheckBox, 0, Qt::AlignRight);
	setLayout(layout);

	this->setObjectName(kTipsWindow);
	m_curMimeDataLabel->setObjectName(kMainLabel);
	m_historyMimeDataListWidget->setObjectName(kLabelList);
	m_expandCheckBox->setObjectName(kExpandBtn);
	m_autoShowCheckBox->setObjectName(kAutoShowBtn);

	m_historyMimeDataListWidget->setSelectionMode(QAbstractItemView::NoSelection);
	m_historyMimeDataListWidget->setItemDelegate(new NoFocusDelegate);
#ifdef WHEEL_SCROLL_PER_PIXEL
	m_historyMimeDataListWidget->verticalScrollBar()->setSingleStep(1);
	m_historyMimeDataListWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
	m_historyMimeDataListWidget->installEventFilter(this);
#endif

	m_autoShowCheckBox->setText(tr("Auto show"));
	m_historyMimeDataListWidget->setVisible(false);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::ToolTip);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

void ClipboardTipsWindow::onHistoryListUpdate()
{
	updateHistoryList();
	this->show();
}

void ClipboardTipsWindow::onExpandStateChanged(int state)
{
	m_historyMimeDataListWidget->setVisible(Qt::CheckState::Unchecked != state);
	adjustSize();
}

void ClipboardTipsWindow::onItemDoubleClicked(QListWidgetItem* item)
{
	MimeDataLabel* label = dynamic_cast<MimeDataLabel*>(m_historyMimeDataListWidget->itemWidget(item));
	if (!label) return;
	label->onDoubleClicked();
}

void ClipboardTipsWindow::enterEvent(QEvent* event)
{
	if (m_timer->isActive()) m_timer->stop();
	__super::enterEvent(event);
}

void ClipboardTipsWindow::leaveEvent(QEvent* event)
{
	if (m_autoShowCheckBox->isChecked())
		m_timer->start(kShowTime * 1000);
	__super::leaveEvent(event);
}

void ClipboardTipsWindow::paintEvent(QPaintEvent *event)
{
	// support qss
	QStyleOption opt;
	opt.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool ClipboardTipsWindow::eventFilter(QObject *watched, QEvent *event)
{
	// TODO
	// wheel scroll per pixel
	if (event->type() == QEvent::Wheel) {
		QWheelEvent *wEvent = static_cast<QWheelEvent*>(event);
		auto scrollbar = dynamic_cast<QScrollBar*>(watched);
		if (scrollbar) {
			auto oriVal = scrollbar->value();
			scrollbar->setValue(wEvent->delta() > 0 ? oriVal - 1 : oriVal + 1);
		}
	}
	return false;
}
