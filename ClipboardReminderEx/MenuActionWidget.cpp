#include "MenuActionWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>

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

void NumMenuActionWidget::initWindow()
{
	m_numLabel = new QLabel(this);
	m_plusButton = new QPushButton(this);
	m_minusButton = new QPushButton(this);
	m_describeTextLabel = new QLabel(this);

	m_describeTextLabel->setText(m_describeText);
	m_numLabel->setText(QString::number(m_curVal));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_describeTextLabel);
	QHBoxLayout* downLayout = new QHBoxLayout(this);
	downLayout->addWidget(m_minusButton);
	downLayout->addWidget(m_numLabel);
	downLayout->addWidget(m_plusButton);
	layout->addLayout(downLayout);
	setLayout(layout);
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
