#pragma once

#include <QWidget>

class QGraphicsView;
class QGraphicsScene;
class CanvasWidget : public QWidget
{
	Q_OBJECT

public:
	CanvasWidget(QWidget *parent);
	void setUp(const QPixmap&);

protected:
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
	virtual bool eventFilter(QObject *watched, QEvent *event) override;

signals:
	void sigFinish();
	void sigQuit();

private:
	void initWindow();

private:
	QGraphicsView* m_view;
	QGraphicsScene* m_scene;
};
