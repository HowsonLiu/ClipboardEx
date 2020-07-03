#pragma once
#include <QWidget>
#include <QPainterPath>

class SnipToolBar;
class MagnifierWidget;

/** @class SnipWindow
  * @brief grab and snip desktop
  * @author HowsonLiu
  * @date 2020/6/21
  */
class SnipWindow : public QWidget
{
	Q_OBJECT
public:
	SnipWindow(QWidget* parent = nullptr);
	void setUp(const QRect& rect, const QPixmap& pixmap);

protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void wheelEvent(QWheelEvent *event) override;

private:
	void initWindow();

private slots:
	void onLoseFocus(Qt::ApplicationState);
	void onRadioToggled(int id, bool status);

private:
	// widget
	SnipToolBar* m_toolbar;
	MagnifierWidget* m_magnifier;

	// paint
	QPixmap m_pixmap;

	enum SnipType {
		kRect,
		kFreeForm,
		kApplication
	} m_snipType = kRect;
	// rect snip
	QRect m_rect;
	bool m_bRectStarted = false;

	// freefrom snip
	QPainterPath m_path;
	bool m_bPathStarted = false;
};

void Snip();