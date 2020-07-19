#pragma once
#include <QWidget>

class SnipWidget;
class CanvasWidget;
class QStackedLayout;
class ScreenshotWindow : public QWidget 
{
	Q_OBJECT
public:
	ScreenshotWindow(QWidget* parent = nullptr);
	void setUp(const QRect& rect, const QPixmap& pixmap);

private:
	void initWindow();
	void switchCanvas();

private slots:
	void onSnipFinish(const QRect&);
	void onSnipFinish(const QPainterPath&);

private:
	QStackedLayout* m_stack;
	SnipWidget* m_snip;
	CanvasWidget* m_canvas;
};

void screenshot();