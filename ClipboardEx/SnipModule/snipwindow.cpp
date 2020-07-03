#include "snipwindow.h"
#include "def.h"
#include "util/floatlayout.h"
#include "sniptoolbar.h"
#include "magnifierwidget.h"
#include <QGuiApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

SnipWindow::SnipWindow(QWidget* parent /*= nullptr*/) : QWidget(parent)
{
	initWindow();
	connect(qApp, &QGuiApplication::applicationStateChanged, this, &SnipWindow::onLoseFocus);
	connect(m_toolbar, &SnipToolBar::sigRadioToggled, this, &SnipWindow::onRadioToggled);
	connect(m_toolbar, &SnipToolBar::sigClose, this, &SnipWindow::close);
}

void SnipWindow::setUp(const QRect& rect, const QPixmap& pixmap)
{
	m_pixmap = pixmap;
	this->setGeometry(rect);
	m_magnifier->setUp(pixmap);
}

void SnipWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		if (m_snipType == kRect) {
			m_rect.setTopLeft(event->pos());
			m_bRectStarted = true;
		}
		else if (m_snipType == kFreeForm) {
			m_path = QPainterPath();
			m_path.moveTo(event->pos());
			m_bPathStarted = true;
		}
	}
	__super::mousePressEvent(event);
}

void SnipWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_snipType == kRect && m_bRectStarted) {
		m_rect.setBottomRight(event->pos());
		repaint();
	}
	else if (m_snipType == kFreeForm && m_bPathStarted) {
		m_path.lineTo(event->pos());
		repaint();
	}
	if (m_magnifier->isVisible())
		m_magnifier->updatePosision(event->pos());
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
		else if (m_snipType == kFreeForm && m_bPathStarted) {
			m_path.lineTo(event->pos());
			m_bPathStarted = false;
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
	path.addPath(m_path);
	painter.setBrush(QBrush(kMaskColor));
	painter.drawPath(path);
	painter.restore();

	// paint rect
	if (!m_rect.isEmpty()) {
		painter.setPen(kRectColor);
		painter.drawRect(m_rect);
	}

	// paint path
	if (!m_path.isEmpty()) {
		painter.setPen(kRectColor);
		painter.drawPath(m_path);
	}
}

void SnipWindow::wheelEvent(QWheelEvent *event)
{
	if (m_magnifier->isVisible()) {
		m_magnifier->updateMagnifySize(event->delta());
		m_magnifier->updatePosision(event->pos());
	}

	__super::wheelEvent(event);
}

void SnipWindow::initWindow()
{
	m_toolbar = new SnipToolBar(this);
	m_magnifier = new MagnifierWidget;

	FloatLayout* fLayout = new FloatLayout(this);
	fLayout->addWidget(m_toolbar, Qt::AlignHCenter | Qt::AlignTop);

	m_magnifier->setVisible(false);

	setMouseTracking(true);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);
}

void SnipWindow::onLoseFocus(Qt::ApplicationState state)
{
	//if (state != Qt::ApplicationState::ApplicationActive)
	//	close();
}

void SnipWindow::onRadioToggled(int id, bool status)
{
	if (!status) return;
	m_snipType = static_cast<SnipType>(id);
	m_magnifier->setVisible(m_snipType != kApplication);
}

void Snip()
{
	QRect screenGeometry = QGuiApplication::primaryScreen()->virtualGeometry();
	QPixmap screenPixmap = QGuiApplication::primaryScreen()->grabWindow(0, screenGeometry.x(), screenGeometry.y(), screenGeometry.width(), screenGeometry.height());
	SnipWindow* snipWindow = new SnipWindow;
	snipWindow->setUp(screenGeometry, std::move(screenPixmap));
	snipWindow->show();
}
