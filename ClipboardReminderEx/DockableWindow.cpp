#include "DockableWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QScreen>
#include <QDebug>

namespace {
	// Window docks until window's edge exceed monitor's edge kDockEnableDistance
	const int kDockEnableDistance = 5;
	// it will show kUnDockTriggerDistance pixel edge when docks
	const int kUnDockTriggerDistance = 3;
	const int kDockAnimationDuration = 200;	// ms
}

DockableWindowState::DockableWindowState(const QString& str)
{
	if (str.isEmpty()) return;
	QStringList list = str.split(',');
	if (list.size() < 3) return;
	screenIndex = list[0].toInt();
	dockDirection = list[1].toInt();
	if (DockDirection::None == dockDirection && list.size() == 4) {
		dockPosition.undockPosion.setX(list[2].toFloat());
		dockPosition.undockPosion.setY(list[3].toFloat());
	}
	else if (DockDirection::None != dockDirection && list.size() == 3) {
		dockPosition.dockOffset = list[2].toFloat();
	}
}

DockableWindowState::operator QString()
{
	QString res = QString::number(screenIndex) + ',' + QString::number(dockDirection) + ',';
	if (DockDirection::None == dockDirection)
		return res + QString::number(dockPosition.undockPosion.x()) + "," + QString::number(dockPosition.undockPosion.y());
	else
		return res + QString::number(dockPosition.dockOffset);
}

DockableWindow::DockableWindow(QWidget *parent)
	: QWidget(parent)
{
	initWindow();
	connect(m_dockShowAnimation, &QPropertyAnimation::finished, this, [this]() {m_bDockShow = true; });
	connect(m_dockHideAnimation, &QPropertyAnimation::finished, this, [this]() {m_bDockShow = false; });
}

DockableWindowState DockableWindow::getDockableState() const
{
	DockableWindowState res;
	res.screenIndex = QApplication::desktop()->screenNumber(this);
	res.dockDirection = m_curDockDirection;
	QRect screenRect = curScreenRect();
	if (Q_UNLIKELY(!screenRect.isValid())) return res;
	float xOffset = (float)(this->x() - screenRect.left()) / screenRect.width();
	float yOffset = (float)(this->y() - screenRect.top()) / screenRect.height();
	if (DockDirection::LEFT == m_curDockDirection || DockDirection::RIGHT == m_curDockDirection) {
		res.dockPosition.dockOffset = yOffset;
	}
	else if (DockDirection::UP == m_curDockDirection) {
		res.dockPosition.dockOffset = xOffset;
	}
	else {
		res.dockPosition.undockPosion.setX(xOffset);
		res.dockPosition.undockPosion.setY(yOffset);
	}
	return res;
}

void DockableWindow::loadDockableState(const DockableWindowState& dockpos)
{
	QScreen* screen = QGuiApplication::screens()[0];
	if (dockpos.screenIndex < QGuiApplication::screens().size() && dockpos.screenIndex >= 0)
		screen = QGuiApplication::screens()[dockpos.screenIndex];
	m_curDockDirection = (DockDirection)dockpos.dockDirection;
	int target_x, target_y;
	QRect screenRect = screen->availableGeometry();
	if (DockDirection::LEFT == m_curDockDirection) {
		target_x = screenRect.left() - this->width() + kUnDockTriggerDistance;
		target_y = dockpos.dockPosition.dockOffset * screenRect.height() + screenRect.top();
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		target_x = screenRect.right() - kUnDockTriggerDistance;
		target_y = dockpos.dockPosition.dockOffset * screenRect.height() + screenRect.top();
	}
	else if (DockDirection::UP == m_curDockDirection) {
		target_y = screenRect.top() - this->height() + kUnDockTriggerDistance;
		target_x = dockpos.dockPosition.dockOffset * screenRect.width() + screenRect.left();
	}
	else {
		target_x = dockpos.dockPosition.undockPosion.x() * screenRect.width() + screenRect.left();
		target_y = dockpos.dockPosition.undockPosion.y() * screenRect.height() + screenRect.top();
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
		dockHide();
}

void DockableWindow::initWindow()
{
	m_dockShowAnimation = new QPropertyAnimation(this, "geometry");
	m_dockShowAnimation->setDuration(kDockAnimationDuration);
	m_dockHideAnimation = new QPropertyAnimation(this, "geometry");
	m_dockHideAnimation->setDuration(kDockAnimationDuration);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

QRect DockableWindow::curScreenRect() const
{
	int screenIndex = QApplication::desktop()->screenNumber(this);
	if (screenIndex < 0) return QRect();
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
	if (m_curDockDirection == dockDirection) return;
	m_curDockDirection = dockDirection;
	emit sigDockStateChanged(m_curDockDirection);
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
		target_x = belongScreenRect.right() - rect.width() + 1;
	move(target_x, target_y);
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
		int target_x = curScreenRect().right() - this->width() + 1;
		dockAnimationShow(target_x, this->y());
	}
}

void DockableWindow::dockHide()
{
	if (Q_UNLIKELY(!curScreenRect().isValid())) return;
	if (DockDirection::None == m_curDockDirection) return;
	if (DockDirection::UP == m_curDockDirection) {
		int target_y = curScreenRect().top() - this->height() + kUnDockTriggerDistance;
		dockAnimationHide(this->x(), target_y);
	}
	else if (DockDirection::LEFT == m_curDockDirection) {
		int target_x = curScreenRect().left() - this->width() + kUnDockTriggerDistance;
		dockAnimationHide(target_x, this->y());
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		int target_x = curScreenRect().right() - kUnDockTriggerDistance;
		dockAnimationHide(target_x, this->y());
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
