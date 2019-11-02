#pragma once

#include <QtWidgets/QWidget>
#include <QPoint>

// Window docks until window's edge exceed monitor's edge DOCK_ENABLE_DISTANCE
#define DOCK_ENABLE_DISTANCE 5
// it will show DOCK_SHOW_DISTANCE pixel edge when docks
#define DOCK_SHOW_DISTANCE 3
#define DOCK_ANIMATION_DURATION 200 // ms

enum DockDirection {
	None = 0,
	UP,
	LEFT,
	RIGHT
};

class QClipboard;
class QMouseEvent;
class QPropertyAnimation;

/*!
 * \class DockableWindowPosition
 * \brief The serialization and deserialization of dockable window postion
 * \author liuhaosheng
 * \date November 2019
 */
struct DockableWindowPosition 
{
	short dockDirection;
	union {
		QPoint  undockPosion;
		int dockOffset;
	} dockPosition{};
};

/*!
 * \class DockableWindow
 *
 * \brief A base class for dockable windows
 *
 * \author HowsonLiu
 * \date October 2019
 */
class DockableWindow : public QWidget
{
	Q_OBJECT

public:
	DockableWindow(QWidget *parent = Q_NULLPTR);
	DockableWindowPosition getDockablePostion() const;
	void moveDockablePosition(const DockableWindowPosition&);

protected:
	// handle drag and dock
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;

private:
	void initWindow();
	// dock
	DockDirection canDock() const;
	void setDock(const DockDirection);
	void prepareDock();
	void dockShow();
	void dockHide();
	void dockAnimationShow(int x, int y);
	void dockAnimationHide(int x, int y);

private:
	// drag and dock variable
	bool m_bIsDraging = false;
	bool m_bDockShow = false;
	DockDirection m_curDockDirection = DockDirection::None;
	QPoint m_mousePressPoint;
	QPropertyAnimation* m_dockShowAnimation = nullptr;
	QPropertyAnimation* m_dockHideAnimation = nullptr;
};
