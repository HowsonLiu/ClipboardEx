#pragma once

#include <QtWidgets/QWidget>
#include <QPoint>

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
struct DockableWindowState 
{
	short screenIndex = 0;
	short dockDirection = DockDirection::None;
	union {
		QPointF  undockPosion;
		float dockOffset;
	} dockPosition = { {0.0f,0.0f} };

public:
	DockableWindowState() {};
	DockableWindowState(const QString&);
	operator QString();
};

/*!
 * \class DockableWindow
 * \brief A base class for dockable windows
 * \author HowsonLiu
 * \date October 2019
 */
class DockableWindow : public QWidget
{
	Q_OBJECT

public:
	DockableWindow(QWidget *parent = Q_NULLPTR);
	DockableWindowState getDockableState() const;
	void loadDockableState(const DockableWindowState&);

protected:
	// handle drag and dock
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;

private:
	void initWindow();
	QRect curScreenRect() const;
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
