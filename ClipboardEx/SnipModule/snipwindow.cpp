#include "snipwindow.h"
#include "def.h"
#include "util/floatlayout.h"
#include <QRadioButton>
#include <QPushButton>
#include <QLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>

SnipWindow::SnipWindow(QWidget* parent /*= nullptr*/) : QWidget(parent)
{
	initWindow();
	connect(qApp, &QGuiApplication::applicationStateChanged, this, &SnipWindow::onLoseFocus);
}

void SnipWindow::setUp(const QRect& rect, const QPixmap& pixmap)
{
	m_pixmap = pixmap;
	this->setGeometry(rect);
}

void SnipWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		if (m_snipType == kRect) {
			m_rect.setTopLeft(event->pos());
			m_bRectStarted = true;
		}
	}
	__super::mousePressEvent(event);
}

void SnipWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_snipType == kRect && m_bRectStarted) {
		qDebug() << m_rect;
		m_rect.setBottomRight(event->pos());
		repaint();
	}
	__super::mouseMoveEvent(event);
}

void SnipWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		if (m_snipType == kRect && m_bRectStarted) {
			m_rect.setBottomRight(event->pos());
			m_bRectStarted = false;
			repaint();
		}
	}
	__super::mouseReleaseEvent(event);
}

void SnipWindow::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.drawPixmap(QPoint(0, 0), m_pixmap);

	// paint mask
	painter.save();
	QPainterPath path;
	path.addRect(this->rect());
	path.addRect(m_rect);
	painter.setBrush(QBrush(kMaskColor));
	painter.drawPath(path);
	painter.restore();

	// paint rect
	painter.setPen(kRectColor);
	painter.drawRect(m_rect);
}

void SnipWindow::initWindow()
{
	m_toolbar = new QWidget(this);
	m_radioButton = new QRadioButton(m_toolbar);
	m_closeButton = new QPushButton(m_toolbar);

	QHBoxLayout* hLayout = new QHBoxLayout(m_toolbar);
	hLayout->addWidget(m_radioButton);
	hLayout->addWidget(m_closeButton);
	m_toolbar->setLayout(hLayout);

	FloatLayout* fLayout = new FloatLayout(this);
	fLayout->addWidget(m_toolbar, Qt::AlignHCenter | Qt::AlignTop);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

void SnipWindow::onLoseFocus(Qt::ApplicationState state)
{
	//if (state != Qt::ApplicationState::ApplicationActive)
	//	close();
}

void Snip()
{
	QRect screenGeometry = QGuiApplication::primaryScreen()->virtualGeometry();
	QPixmap screenPixmap = QGuiApplication::primaryScreen()->grabWindow(0, screenGeometry.x(), screenGeometry.y(), screenGeometry.width(), screenGeometry.height());
	SnipWindow* snipWindow = new SnipWindow;
	snipWindow->setUp(screenGeometry, screenPixmap);
	snipWindow->show();
}
