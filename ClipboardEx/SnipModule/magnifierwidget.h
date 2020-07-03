#ifndef MAGNIFIERWIDGET_H
#define MAGNIFIERWIDGET_H
#include <QWidget>
#include <QLabel>
#include <QImage>

/** @class MagnifierLabel
  * @brief Pixmap label. can zoom in and zoom out
  * @copyright (c) 2020, HowsonLiu. All rights reserved
  * @author HowsonLiu
  * @date 2020/6/30
  */
class MagnifierLabel : public QLabel 
{
public:
	MagnifierLabel(QWidget* parent = nullptr);
	void setUp(const QPixmap& p);
	void updatePosision(const QPoint& p);
	void updateMagnifySize(int delta);

protected:
	virtual void paintEvent(QPaintEvent * event) override;

private:
	QPixmap m_originPixmap;
	QSize m_cutSize;
	QRect m_cutRect;
};

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
	QImage m_image;
	MagnifierLabel* m_pixmapLabel;
	QLabel* m_infoLabel;
};

#endif // MAGNIFIERWIDGET_H
