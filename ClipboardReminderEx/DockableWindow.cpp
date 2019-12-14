#include "DockableWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QScreen>
#include <QDebug>

DockableWindowState::DockableWindowState() : dockDirection(DockDirection::None), dockPosition({ {0,0} })
{
}


DockableWindowState::DockableWindowState(const QString& str)
{
	if (str.isEmpty()) return;
	QStringList list = str.split(",");
	if (list.isEmpty()) return;
	if (DockDirection::None == list[0].toInt() && list.size() == 3) {
		dockDirection = list[0].toInt();
		dockPosition.undockPosion.setX(list[1].toInt());
		dockPosition.undockPosion.setY(list[2].toInt());
	}
	else if (DockDirection::None != list[0].toInt() && list.size() == 2) {
		dockDirection = list[0].toInt();
		dockPosition.dockOffset = list[1].toInt();
	}
}

DockableWindowState::operator QString()
{
	QString res = QString::number(dockDirection) + ",";
	if (DockDirection::None == dockDirection)
		return res + QString::number(dockPosition.undockPosion.x()) + "," + QString::number(dockPosition.undockPosion.y());
	else
		return res + QString::number(dockPosition.dockOffset);
}

DockableWindow::DockableWindow(QWidget *parent)
	: QWidget(parent)
{
	initWindow();
	connect(m_dockShowAnimation, &QPropertyAnimation::finished, [this]() {m_bDockShow = true; });
	connect(m_dockHideAnimation, &QPropertyAnimation::finished, [this]() {m_bDockShow = false; });
}

DockableWindowState DockableWindow::getDockableState() const
{
	DockableWindowState res;
	res.dockDirection = m_curDockDirection;
	if (DockDirection::LEFT == m_curDockDirection || DockDirection::RIGHT == m_curDockDirection)
		res.dockPosition.dockOffset = this->y();
	else if (DockDirection::UP == m_curDockDirection)
		res.dockPosition.dockOffset = this->x();
	else 
		res.dockPosition.undockPosion = this->pos();
	return res;
}

void DockableWindow::loadDockableState(const DockableWindowState& dockpos)
{
	m_curDockDirection = (DockDirection)dockpos.dockDirection;
	int target_x, target_y;
	if (DockDirection::LEFT == m_curDockDirection) {
		target_x = GetSystemMetrics(SM_XVIRTUALSCREEN) - this->width() + DOCK_SHOW_DISTANCE;
		target_y = dockpos.dockPosition.dockOffset;
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		int screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		int screen_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		target_x = screen_x + screen_width - DOCK_SHOW_DISTANCE;
		target_y = dockpos.dockPosition.dockOffset;
	}
	else if (DockDirection::UP == m_curDockDirection) {
		target_y = GetSystemMetrics(SM_YVIRTUALSCREEN) - this->height() + DOCK_SHOW_DISTANCE;
		target_x = dockpos.dockPosition.dockOffset;
	}
	else {
		target_x = dockpos.dockPosition.undockPosion.x();
		target_y = dockpos.dockPosition.undockPosion.y();
	}
	move(target_x, target_y);
}

void DockableWindow::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button()) {
		m_bIsDraging = true;
		m_mousePressPoint = event->pos();
	}
	__super::mousePressEvent(event);
}

void DockableWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_bIsDraging) move(event->pos() - m_mousePressPoint + pos());
	__super::mouseMoveEvent(event);
}

void DockableWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_bIsDraging = false;
	setDock(canDock());
	if (DockDirection::None != m_curDockDirection) 
		prepareDock();
	__super::mouseReleaseEvent(event);
}

void DockableWindow::enterEvent(QEvent* event)
{
	if (!m_bDockShow && QAbstractAnimation::Running != m_dockShowAnimation->state()
		&& QAbstractAnimation::Running != m_dockHideAnimation->state()) {
		dockShow();
	}
}

void DockableWindow::leaveEvent(QEvent* event)
{
	if (DockDirection::None != m_curDockDirection)
		dockHide(true);
}

void DockableWindow::resizeEvent(QResizeEvent* event)
{
	__super::resizeEvent(event);
	dockHide(false);
}

void DockableWindow::initWindow()
{
	m_dockShowAnimation = new QPropertyAnimation(this, "geometry");
	m_dockShowAnimation->setDuration(DOCK_ANIMATION_DURATION);
	m_dockHideAnimation = new QPropertyAnimation(this, "geometry");
	m_dockHideAnimation->setDuration(DOCK_ANIMATION_DURATION);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

QRect DockableWindow::curScreenRect() const
{
	int screenIndex = QApplication::desktop()->screenNumber(this);
	QScreen* curScreen = QGuiApplication::screens()[screenIndex];
	if (curScreen) return curScreen->availableGeometry();
	return QRect();
}

DockDirection DockableWindow::canDock() const
{
	QScreen* topLeftScreen = QGuiApplication::screenAt(this->geometry().topLeft());
	QScreen* topRightScreen = QGuiApplication::screenAt(this->geometry().topRight());
	QScreen* bottomLeftScreen = QGuiApplication::screenAt(this->geometry().bottomLeft());
	QScreen* bottomRightScreen = QGuiApplication::screenAt(this->geometry().bottomRight());
	if (!topLeftScreen && topLeftScreen == topRightScreen) return DockDirection::UP;
	else if (!topLeftScreen && topLeftScreen == bottomLeftScreen) return DockDirection::LEFT;
	else if (!topRightScreen && topRightScreen == bottomRightScreen) return DockDirection::RIGHT;
	return DockDirection::None;
}

void DockableWindow::setDock(const DockDirection dockDirection)
{
	m_curDockDirection = dockDirection;
}

void DockableWindow::prepareDock()
{
	if (Q_UNLIKELY(!curScreenRect().isValid())) return;
	// Make window show on the belong screen fully
	QRect belongScreenRect = curScreenRect();
	QRect rect = this->geometry();
	int target_y = rect.y() > belongScreenRect.y() ? rect.y() : belongScreenRect.y();
	int target_x = rect.x();
	if (belongScreenRect.left() > rect.left())
		target_x = belongScreenRect.left();
	else if (belongScreenRect.right() < rect.right())
		target_x = belongScreenRect.right() - rect.width();
	move(target_x, target_y);
	qDebug() << belongScreenRect << rect << belongScreenRect.right() << belongScreenRect.left();
}

void DockableWindow::dockShow()
{
	if (Q_UNLIKELY(!curScreenRect().isValid())) return;
	if (DockDirection::None == m_curDockDirection) return;
	if (DockDirection::UP == m_curDockDirection) {
		dockAnimationShow(this->x(), curScreenRect().top());
	}
	else if (DockDirection::LEFT == m_curDockDirection) {
		dockAnimationShow(curScreenRect().left(), this->y());
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		int target_x = curScreenRect().right() - this->width();
		dockAnimationShow(target_x, this->y());
	}
}

void DockableWindow::dockHide(bool animation)
{
	if (Q_UNLIKELY(!curScreenRect().isValid())) return;
	if (DockDirection::None == m_curDockDirection) return;
	if (DockDirection::UP == m_curDockDirection) {
		int target_y = curScreenRect().top() - this->height() + DOCK_SHOW_DISTANCE;
		if (animation) dockAnimationHide(this->x(), target_y);
		else move(this->x(), target_y);
	}
	else if (DockDirection::LEFT == m_curDockDirection) {
		int target_x = curScreenRect().left() - this->width() + DOCK_SHOW_DISTANCE;
		if (animation) dockAnimationHide(target_x, this->y());
		else move(target_x, this->y());
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		int target_x = curScreenRect().right() - DOCK_SHOW_DISTANCE;
		if (animation) dockAnimationHide(target_x, this->y());
		else move(target_x, this->y());
	}
}

void DockableWindow::dockAnimationShow(int x, int y)
{
	auto end_val = this->geometry();
	end_val.moveTo(x, y);
	m_dockShowAnimation->setStartValue(this->geometry());
	m_dockShowAnimation->setEndValue(end_val);
	m_dockShowAnimation->start();
}

void DockableWindow::dockAnimationHide(int x, int y)
{
	auto end_val = this->geometry();
	end_val.moveTo(x, y);
	m_dockHideAnimation->setStartValue(this->geometry());
	m_dockHideAnimation->setEndValue(end_val);
	m_dockHideAnimation->start();
}
