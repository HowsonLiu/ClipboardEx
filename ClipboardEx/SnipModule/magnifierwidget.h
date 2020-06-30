#ifndef MAGNIFIERWIDGET_H
#define MAGNIFIERWIDGET_H
#include <QWidget>

class QLabel;

/** @class MagnifierWidget
  * @brief Magnifier
  * @copyright (c) 2020, HowsonLiu. All rights reserved
  * @author HowsonLiu
  * @date 2020/6/28
  */
class MagnifierWidget : public QWidget
{
public:
    MagnifierWidget(QWidget* parent = nullptr);
	void setUp(const QPixmap& p);
	void updatePosision(const QPoint& p);
	void updateMagnifySize(int delta);

private:
	void initWindow();

private:
	QPixmap m_pixmap;
	QLabel* m_pixmapLabel;
	QLabel* m_infoLabel;

	QSize m_originSize;
};

#endif // MAGNIFIERWIDGET_H
