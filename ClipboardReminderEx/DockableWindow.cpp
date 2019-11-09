#include "DockableWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>
#include <Windows.h>
#include <WinUser.h>
#include <QPropertyAnimation>
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

DockDirection DockableWindow::canDock() const
{
	int screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int screen_y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int screen_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int screen_height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	// monitor with different resolutions combination will cause calculation not accurate
	// like 1980x1080 + 2550x1440
	// 1980x1080 dock upside will miss
	// NOT TODO
	if (this->y() < screen_y - DOCK_ENABLE_DISTANCE) return DockDirection::UP;	// up direction has high priority
	if (this->x() < screen_x - DOCK_ENABLE_DISTANCE) return DockDirection::LEFT;
	if (this->x() + this->width() > screen_x + screen_width) return DockDirection::RIGHT;
	return DockDirection::None;
}

void DockableWindow::setDock(const DockDirection dockDirection)
{
	m_curDockDirection = dockDirection;
}

void DockableWindow::prepareDock()
{
	// In some case, the windows exceeds more than one direction, so we need to make sure the window fully shows
	int screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int screen_y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int screen_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int target_y = this->y() < screen_y ? screen_y : this->y();
	int target_x = this->x();
	if (this->x() < screen_x) target_x = screen_x;
	else if (this->x() > screen_x + screen_width - this->width())
		target_x = screen_x + screen_width - this->width();
	move(target_x, target_y);
}

void DockableWindow::dockShow()
{
	if (DockDirection::None == m_curDockDirection) return;
	if (DockDirection::UP == m_curDockDirection) {
		dockAnimationShow(this->x(), GetSystemMetrics(SM_YVIRTUALSCREEN));
	}
	else if (DockDirection::LEFT == m_curDockDirection) {
		dockAnimationShow(GetSystemMetrics(SM_XVIRTUALSCREEN), this->y());
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		int screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		int screen_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int target_x = screen_x + screen_width - this->width();
		dockAnimationShow(target_x, this->y());
	}
}

void DockableWindow::dockHide(bool animation)
{
	if (DockDirection::None == m_curDockDirection) return;
	if (DockDirection::UP == m_curDockDirection) {
		int target_y = GetSystemMetrics(SM_YVIRTUALSCREEN) - this->height() + DOCK_SHOW_DISTANCE;
		if (animation) dockAnimationHide(this->x(), target_y);
		else move(this->x(), target_y);
	}
	else if (DockDirection::LEFT == m_curDockDirection) {
		int target_x = GetSystemMetrics(SM_XVIRTUALSCREEN) - this->width() + DOCK_SHOW_DISTANCE;
		if (animation) dockAnimationHide(target_x, this->y());
		else move(target_x, this->y());
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		int screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		int screen_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int target_x = screen_x + screen_width - DOCK_SHOW_DISTANCE;
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
