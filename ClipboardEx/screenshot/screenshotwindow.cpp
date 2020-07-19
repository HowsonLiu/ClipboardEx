#include "screenshotwindow.h"
#include "screenshot/snip/snipwidget.h"
#include "screenshot/canvas/canvaswidget.h"
#include <QGuiApplication>
#include <QScreen>
#include <QStackedLayout>
#include <QDebug>

ScreenshotWindow::ScreenshotWindow(QWidget* parent) : QWidget(parent)
{
	initWindow();
	connect(m_snip, &SnipWidget::sigQuit, this, &ScreenshotWindow::close);
	connect(m_snip, static_cast<void(SnipWidget::*)(const QPainterPath&)>(&SnipWidget::sigFinish),
		this, static_cast<void(ScreenshotWindow::*)(const QPainterPath&)>(&ScreenshotWindow::onSnipFinish));
	connect(m_snip, static_cast<void(SnipWidget::*)(const QRect&)>(&SnipWidget::sigFinish),
		this, static_cast<void(ScreenshotWindow::*)(const QRect&)>(&ScreenshotWindow::onSnipFinish));
	connect(m_canvas, &CanvasWidget::sigQuit, this, [this]() {m_stack->setCurrentIndex(0); });
}

void ScreenshotWindow::setUp(const QRect& rect, const QPixmap& pixmap)
{
	this->setGeometry(rect);
	m_snip->setUp(rect, pixmap);
}

void ScreenshotWindow::initWindow()
{
	m_stack = new QStackedLayout(this);
	m_snip = new SnipWidget(this);
	m_canvas = new CanvasWidget(this);

	m_stack->addWidget(m_snip);
	m_stack->addWidget(m_canvas);
	setLayout(m_stack);

	setMouseTracking(true);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);
}

void ScreenshotWindow::switchCanvas()
{
	m_stack->setCurrentIndex(1);
	auto pixmap = m_snip->grab(m_snip->rect());
	m_canvas->setUp(pixmap);
}

void ScreenshotWindow::onSnipFinish(const QRect& r)
{
	switchCanvas();
}

void ScreenshotWindow::onSnipFinish(const QPainterPath& p)
{
	switchCanvas();
}

void screenshot()
{
	QRect screenGeometry = QGuiApplication::primaryScreen()->virtualGeometry();
	QPixmap screenPixmap = QGuiApplication::primaryScreen()->grabWindow(0, screenGeometry.x(), screenGeometry.y(), screenGeometry.width(), screenGeometry.height());
	ScreenshotWindow* w = new ScreenshotWindow;
	w->setUp(screenGeometry, std::move(screenPixmap));
	w->show();
}
