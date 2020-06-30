#include "magnifierwidget.h"
#include "def.h"
#include <QLabel>
#include <QLayout>
#include <QEvent>
#include <QPainter>
#include <QScreen>
#include <QApplication>

MagnifierWidget::MagnifierWidget(QWidget* parent /*= nullptr*/)
	: QWidget(parent)
	, m_originSize(kMaxMagnifySize)
{
	initWindow();
}

void MagnifierWidget::setUp(const QPixmap& p)
{
	m_pixmap = p;
}

void MagnifierWidget::updatePosision(const QPoint& p)
{
	// draw magnifier
	QRect originRect;
	originRect.setSize(m_originSize);
	originRect.moveCenter(p);
	QPixmap magnifyPixmap = m_pixmap.copy(originRect).scaled(kWidgetSize, Qt::KeepAspectRatio);
	// draw aim point
	QPainter painter(&magnifyPixmap);
	painter.setPen(QPen(kMagnifierPenColor, kMagnifierPenWidth));
	QRect border = magnifyPixmap.rect();
	QLine hline(border.left(), border.center().y(), border.right(), border.center().y());
	QLine vline(border.center().x(), border.top(), border.center().x(), border.bottom());
	painter.drawLine(hline);
	painter.drawLine(vline);
	border.setBottomRight({ border.bottom() - 1, border.right() - 1 });
	painter.drawRect(border);
	m_pixmapLabel->setPixmap(magnifyPixmap);

	// info
	QRgb pixelColor = m_pixmap.toImage().pixel(p);
	QString tips = kMagnifierTips.arg(p.x(), 4).arg(p.y(), 4)
		.arg(qRed(pixelColor), 3).arg(qGreen(pixelColor), 3).arg(qBlue(pixelColor), 3)
		.arg(pixelColor, 0, 16);
	m_infoLabel->setText(tips);
	
	// pos
	QRect magnifyRect = this->rect();
	QRect screenRect = QGuiApplication::screenAt(p)->geometry();
	do {
		// right bottom
		magnifyRect.moveTo(p);
		magnifyRect.translate(kDistanceToCursor, kDistanceToCursor);
		if (screenRect.contains(magnifyRect)) {
			break;
		}
		// left bottom
		magnifyRect.moveTopRight(p);
		magnifyRect.translate(-kDistanceToCursor, kDistanceToCursor);
		if (screenRect.contains(magnifyRect)) {
			break;
		}
		// right top
		magnifyRect.moveBottomLeft(p);
		magnifyRect.translate(kDistanceToCursor, -kDistanceToCursor);
		if (screenRect.contains(magnifyRect)) {
			break;
		}
		// left top
		magnifyRect.moveBottomRight(p);
		magnifyRect.translate(-kDistanceToCursor, -kDistanceToCursor);
		if (screenRect.contains(magnifyRect)) {
			break;
		}
	} while (false);
	this->move(magnifyRect.topLeft());
}

void MagnifierWidget::updateMagnifySize(int delta)
{
	if (delta == 0) return;
	if (delta > 0 && (m_originSize.width() <= kMinMagnifySize.width() || m_originSize.height() <= kMinMagnifySize.height())) return;
	if (delta < 0 && (m_originSize.width() >= kMaxMagnifySize.width() || m_originSize.height() >= kMaxMagnifySize.height())) return;
	if (delta > 0) {
		m_originSize.rwidth() -= kMagnifyResizeStep;
		m_originSize.rheight() -= kMagnifyResizeStep;
	}
	else {
		m_originSize.rwidth() += kMagnifyResizeStep;
		m_originSize.rheight() += kMagnifyResizeStep;
	}
}

void MagnifierWidget::initWindow()
{
	m_pixmapLabel = new QLabel(this);
	m_infoLabel = new QLabel(this);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setMargin(0);
	vLayout->addWidget(m_pixmapLabel);
	vLayout->addWidget(m_infoLabel);
	setLayout(vLayout);

	m_pixmapLabel->setFixedSize(kWidgetSize);
	m_infoLabel->setWordWrap(true);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}
