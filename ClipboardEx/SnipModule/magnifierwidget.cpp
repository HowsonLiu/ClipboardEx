#include "magnifierwidget.h"
#include "def.h"
#include <QLabel>
#include <QLayout>
#include <QEvent>
#include <QPainter>
#include <QScreen>
#include <QApplication>

MagnifierLabel::MagnifierLabel(QWidget* parent /*= nullptr*/)
	: QLabel(parent)
	, m_cutSize(kMaxMagnifySize)
{
	setFixedSize(kWidgetSize);
}

void MagnifierLabel::setUp(const QPixmap& p)
{
	m_originPixmap = p;
}

void MagnifierLabel::updatePosision(const QPoint& p)
{
	m_cutRect.setSize(m_cutSize);
	m_cutRect.moveCenter(p);
	repaint();
}

void MagnifierLabel::updateMagnifySize(int delta)
{
	if (delta == 0) return;
	if (delta > 0 && (m_cutSize.width() <= kMinMagnifySize.width() || m_cutSize.height() <= kMinMagnifySize.height())) return;
	if (delta < 0 && (m_cutSize.width() >= kMaxMagnifySize.width() || m_cutSize.height() >= kMaxMagnifySize.height())) return;
	if (delta > 0) {
		m_cutSize.rwidth() -= kMagnifyResizeStep;
		m_cutSize.rheight() -= kMagnifyResizeStep;
	}
	else {
		m_cutSize.rwidth() += kMagnifyResizeStep;
		m_cutSize.rheight() += kMagnifyResizeStep;
	}
}

void MagnifierLabel::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.setBrush(Qt::black);
	painter.drawRect(this->rect());

	painter.drawPixmap(this->rect(), m_originPixmap, m_cutRect);

	// aim point
	qDebug() << this->rect();
	QLine hline(this->rect().left(), this->rect().center().y(), this->rect().right(), this->rect().center().y());
	QLine vline(this->rect().center().x(), this->rect().top(), this->rect().center().x(), this->rect().bottom());
	painter.setPen(QPen(kMagnifierPenColor, kMagnifierPenWidth / 2));
	painter.drawLine(hline);
	painter.drawLine(vline);

	painter.setBrush(QBrush());
	painter.setPen(QPen(kMagnifierPenColor, kMagnifierPenWidth));
	painter.drawRect(this->rect());
}

MagnifierWidget::MagnifierWidget(QWidget* parent /*= nullptr*/)
	: QWidget(parent)
{
	initWindow();
}

void MagnifierWidget::setUp(const QPixmap& p)
{
	m_image = p.toImage();
	m_pixmapLabel->setUp(p);
}

void MagnifierWidget::updatePosision(const QPoint& p)
{
	// draw magnifier
	m_pixmapLabel->updatePosision(p);

	// info
	QRgb pixelColor = m_image.pixel(p);
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
	m_pixmapLabel->updateMagnifySize(delta);
}

void MagnifierWidget::initWindow()
{
	m_pixmapLabel = new MagnifierLabel(this);
	m_infoLabel = new QLabel(this);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setMargin(0);
	vLayout->addWidget(m_pixmapLabel);
	vLayout->addWidget(m_infoLabel);
	setLayout(vLayout);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}