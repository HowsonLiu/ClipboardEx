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
class SnipWidget : public QWidget
{
	Q_OBJECT
public:
	SnipWidget(QWidget* parent = nullptr);
	~SnipWidget();
	void setUp(const QRect& rect, const QPixmap& pixmap);

protected:
	virtual void showEvent(QShowEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void wheelEvent(QWheelEvent *event) override;
	virtual QSize sizeHint() const override;

private:
	void initWindow();

signals:
	void sigQuit();
	void sigFinish(const QRect&);
	void sigFinish(const QPainterPath&);

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