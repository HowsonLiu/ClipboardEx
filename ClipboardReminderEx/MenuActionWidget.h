#pragma once
#include <QWidget>

class QLabel;
class QPushButton;
class QSlider;

/*!
 * \class NumMenuActionWidget
 *
 * \brief A widget bound to the tray menu, it contains a int val limits in [minVal, maxVal].
 *			which can be change by two buttons
 *
 * \author HowsonLiu
 * \date October 2019
 */
class NumMenuActionWidget : public QWidget
{
	Q_OBJECT
public:
	NumMenuActionWidget(const QString& describeText, int minVal, int defaultVal, int maxVal, QWidget* parent = nullptr);

private:
	void initWindow();

signals:
	void sigNumChange(int);

private slots:
	void onPlusButtonClick();
	void onMinusButtonClick();

private:
	int m_minVal;
	int m_maxVal;
	int m_curVal;
	QString m_describeText;

	QLabel* m_describeTextLabel;
	QLabel* m_numLabel;
	QPushButton* m_plusButton;
	QPushButton* m_minusButton;
};


class SliderMenuActionWidget : public QWidget 
{
	Q_OBJECT
public:
	SliderMenuActionWidget(const QString& describeText, int minVal, int defaultVal, int maxVal, QWidget* parent = nullptr);

private:
	void initWindow();

signals:
	void sigNumChange(int);

private:
	int m_minVal;
	int m_maxVal;
	int m_curVal;
	QString m_describeText;

	QSlider* m_slider;
	QLabel* m_describeTextLabel;
};

class DoubleSliderMenuActionWidget : public QWidget
{
	Q_OBJECT
public:
	DoubleSliderMenuActionWidget(const QString& title, const QString& describeText1, int minVal1, int defaultVal1, int maxVal1,
		const QString& describeText2, int minVal2, int defaultVal2, int maxVal2, QWidget* parent = nullptr);

private:
	void initWindow();

signals:
	void sigNumChange1(int);
	void sigNumChange2(int);

private:
	int m_minVal1;
	int m_maxVal1;
	int m_curVal1;
	QString m_describeText1;
	int m_minVal2;
	int m_maxVal2;
	int m_curVal2;
	QString m_describeText2;
	QString m_titleText;

	QSlider* m_slider1;
	QSlider* m_slider2;
	QLabel* m_titleLabel;
	QLabel* m_describeLabel1;
	QLabel* m_describeLabel2;
};