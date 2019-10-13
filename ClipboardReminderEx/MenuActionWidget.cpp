#include "MenuActionWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

NumMenuActionWidget::NumMenuActionWidget(const QString& describeText, int defaultVal, 
	int minVal, int maxVal, QWidget* parent /*= nullptr*/)
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
