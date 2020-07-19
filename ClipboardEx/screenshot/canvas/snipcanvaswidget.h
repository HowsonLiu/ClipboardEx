#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

class SnipCanvasWidget : public QWidget
{
	Q_OBJECT

public:
	SnipCanvasWidget(QWidget *parent);
	~SnipCanvasWidget();

	enum DrawType {
		kPen,
		kRect,
		kCircle,
		kText
	};

private:
	QGraphicsView* m_view;
	QGraphicsScene* m_scene;
};
