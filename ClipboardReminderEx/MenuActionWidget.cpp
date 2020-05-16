#include "MenuActionWidget.h"
#include "def.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>

NumMenuActionWidget::NumMenuActionWidget(const QString& describeText, int minVal, 
	int defaultVal, int maxVal, QWidget* parent /*= nullptr*/)
	: QWidget(parent)
	, m_minVal(minVal)
	, m_maxVal(maxVal)
	, m_curVal(defaultVal)
	, m_describeText(describeText)
{
	initWindow();
	connect(m_plusButton, &QPushButton::clicked, this, &NumMenuActionWidget::onPlusButtonClick);
	connect(m_minusButton, &QPushButton::clicked, this, &NumMenuActionWidget::onMinusButtonClick);
}

void NumMenuActionWidget::paintEvent(QPaintEvent *event)
{
	// custom qss
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void NumMenuActionWidget::mouseReleaseEvent(QMouseEvent *event)
{
	// be empty to prevent closing when click this widget on menu
}

void NumMenuActionWidget::initWindow()
{
	m_numLabel = new QLabel(this);
	m_plusButton = new QPushButton(this);
	m_minusButton = new QPushButton(this);
	m_describeTextLabel = new QLabel(this);

	m_numLabel->setObjectName(kMenuSubValue);
	m_describeTextLabel->setObjectName(kMenuSubTitle);
	m_plusButton->setObjectName(kMenuSubRBtn);
	m_minusButton->setObjectName(kMenuSubLBtn);
	this->setObjectName(kMenuSubWidget);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_describeTextLabel, 0, Qt::AlignHCenter);
	QHBoxLayout* downLayout = new QHBoxLayout(this);
	downLayout->addWidget(m_minusButton, 0, Qt::AlignVCenter);
	downLayout->addWidget(m_numLabel, 1, Qt::AlignVCenter);
	downLayout->addWidget(m_plusButton, 0, Qt::AlignVCenter);
	layout->addLayout(downLayout, 1);
	setLayout(layout);

	m_describeTextLabel->setText(m_describeText);
	m_numLabel->setText(QString::number(m_curVal));
	m_numLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_minusButton->setEnabled(m_curVal > m_minVal);
	m_plusButton->setEnabled(m_curVal < m_maxVal);

	this->setMouseTracking(true);		// or else menu hover state will not be sync
}

void NumMenuActionWidget::onPlusButtonClick()
{
	if (m_curVal >= m_maxVal) return;
	if (++m_curVal == m_maxVal) m_plusButton->setEnabled(false);
	if (m_curVal > m_minVal) m_minusButton->setEnabled(true);
	m_numLabel->setText(QString::number(m_curVal));
	sigNumChange(m_curVal);
}

void NumMenuActionWidget::onMinusButtonClick()
{
	if (m_curVal <= m_minVal) return;
	if (--m_curVal == m_minVal) m_minusButton->setEnabled(false);
	if (m_curVal < m_maxVal) m_plusButton->setEnabled(true);
	m_numLabel->setText(QString::number(m_curVal));
	sigNumChange(m_curVal);
	qDebug() << styleSheet();
}

SliderMenuActionWidget::SliderMenuActionWidget(const QString& describeText, int minVal, 
	int defaultVal, int maxVal, QWidget* parent /*= nullptr*/)
	: QWidget(parent)
	, m_minVal(minVal)
	, m_curVal(defaultVal)
	, m_maxVal(maxVal)
	, m_describeText(describeText)
{
	initWindow();
	connect(m_slider, &QSlider::valueChanged, this, &SliderMenuActionWidget::sigNumChange);
}

void SliderMenuActionWidget::initWindow()
{
	m_describeTextLabel = new QLabel(this);
	m_slider = new QSlider(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_describeTextLabel);
	layout->addWidget(m_slider);
	setLayout(layout);

	m_slider->setMinimum(m_minVal);
	m_slider->setMaximum(m_maxVal);
	m_slider->setValue(m_curVal);
	m_slider->setOrientation(Qt::Horizontal);
	m_describeTextLabel->setText(m_describeText);
}

DoubleSliderMenuActionWidget::DoubleSliderMenuActionWidget(const QString& title, 
	const QString& describeText1, int minVal1, int defaultVal1, int maxVal1, 
	const QString& describeText2, int minVal2, int defaultVal2, int maxVal2, 
	QWidget* parent /*= nullptr*/) : QWidget(parent), m_titleText(title),
	m_describeText1(describeText1), m_minVal1(minVal1), m_curVal1(defaultVal1), m_maxVal1(maxVal1),
	m_describeText2(describeText2), m_minVal2(minVal2), m_curVal2(defaultVal2), m_maxVal2(maxVal2)
{
	initWindow();
	connect(m_slider1, &QSlider::valueChanged, this, &DoubleSliderMenuActionWidget::sigNumChange1);
	connect(m_slider2, &QSlider::valueChanged, this, &DoubleSliderMenuActionWidget::sigNumChange2);
}

void DoubleSliderMenuActionWidget::initWindow()
{
	m_titleLabel = new QLabel(this);
	m_describeLabel1 = new QLabel(this);
	m_describeLabel2 = new QLabel(this);
	m_slider1 = new QSlider(this);
	m_slider2 = new QSlider(this);
	
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_titleLabel);
	QHBoxLayout* hlayout1 = new QHBoxLayout(this);
	hlayout1->addWidget(m_describeLabel1);
	hlayout1->addWidget(m_slider1);
	QHBoxLayout* hlayout2 = new QHBoxLayout(this);
	hlayout2->addWidget(m_describeLabel2);
	hlayout2->addWidget(m_slider2);
	layout->addLayout(hlayout1);
	layout->addLayout(hlayout2);
	setLayout(layout);

	m_slider1->setMinimum(m_minVal1);
	m_slider1->setMaximum(m_maxVal1);
	m_slider1->setValue(m_curVal1);
	m_slider1->setOrientation(Qt::Horizontal);
	m_describeLabel1->setText(m_describeText1);
	m_slider2->setMinimum(m_minVal2);
	m_slider2->setMaximum(m_maxVal2);
	m_slider2->setValue(m_curVal2);
	m_slider2->setOrientation(Qt::Horizontal);
	m_describeLabel2->setText(m_describeText2);
	m_titleLabel->setText(m_titleText);
}
