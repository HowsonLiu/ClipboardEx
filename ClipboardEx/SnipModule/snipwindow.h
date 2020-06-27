#pragma once
#include <QWidget>

class QRadioButton;
class QPushButton;

/** @class SnipWindow
  * @brief grab and snip desktop
  * @copyright (c) 2020, NetEase Inc. All rights reserved
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

private:
	void initWindow();

private slots:
	void onLoseFocus(Qt::ApplicationState);

private:
	// widget
	QWidget* m_toolbar;
	QRadioButton* m_radioButton;
	QPushButton* m_closeButton;

	// paint
	QPixmap m_pixmap;

	enum SnipType {
		kRect,
		kFreeForm,
		kApplication
	} m_snipType;
	// rect snip
	QRect m_rect;
	bool m_bRectStarted;
};

void Snip();