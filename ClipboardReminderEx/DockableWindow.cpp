#include "DockableWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>
#include <Windows.h>
#include <WinUser.h>
#include <QPropertyAnimation>
#include <QDebug>

DockableWindow::DockableWindow(QWidget *parent)
	: QWidget(parent)
{
	initWindow();
	connect(m_dockShowAnimation, &QPropertyAnimation::finished, [this]() {m_bDockShow = true; });
	connect(m_dockHideAnimation, &QPropertyAnimation::finished, [this]() {m_bDockShow = false; });
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

void DockableWindow::dockHide()
{
	if (DockDirection::None == m_curDockDirection) return;
	if (DockDirection::UP == m_curDockDirection) {
		int target_y = GetSystemMetrics(SM_YVIRTUALSCREEN) - this->height() + DOCK_SHOW_DISTANCE;
		dockAnimationHide(this->x(), target_y);
	}
	else if (DockDirection::LEFT == m_curDockDirection) {
		int target_x = GetSystemMetrics(SM_XVIRTUALSCREEN) - this->width() + DOCK_SHOW_DISTANCE;
		dockAnimationHide(target_x, this->y());
	}
	else if (DockDirection::RIGHT == m_curDockDirection) {
		int screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		int screen_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int target_x = screen_x + screen_width - DOCK_SHOW_DISTANCE;
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
