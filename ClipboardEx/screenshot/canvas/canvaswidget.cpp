#include "canvaswidget.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QLayout>
#include <QDebug>
#include <QGraphicsItem>

CanvasWidget::CanvasWidget(QWidget *parent)
	: QWidget(parent)
{
	initWindow();
}


void CanvasWidget::setUp(const QPixmap& p)
{
	auto pixmapItem = m_scene->addPixmap(p);
	pixmapItem->moveBy(0, 0);
	qDebug() << m_view->contentsMargins();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton) {
			emit sigQuit();
	}
	__super::mouseReleaseEvent(event);
}

void CanvasWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		emit sigFinish();
	}
	__super::mouseDoubleClickEvent(event);
}

bool CanvasWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == m_view) {
		if (event->type() == QEvent::MouseButtonDblClick) {
			emit sigFinish();
		}
		else if (event->type() == QEvent::ContextMenu) {	// right click
			emit sigQuit();
		}
	}

	return true;
}

void CanvasWidget::initWindow()
{
	m_view = new QGraphicsView(this);
	m_scene = new QGraphicsScene(this);

	m_view->setScene(m_scene);
	// we must set frame style, or else default style will set frame's line width 1
	m_view->setFrameStyle(m_view->frameStyle() ^ QFrame::StyledPanel /*| QFrame::Sunken*/);
	m_view->installEventFilter(this);
	// m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout* l = new QVBoxLayout(this);
	l->addWidget(m_view);
	l->setMargin(0);
	setLayout(l);
}
