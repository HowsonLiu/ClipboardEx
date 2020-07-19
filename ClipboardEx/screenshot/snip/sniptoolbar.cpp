#include "sniptoolbar.h"
#include "def.h"
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QLayout>
#include <QPropertyAnimation>
#include <QTimer>

SnipToolBar::SnipToolBar(QWidget* parent /*= nullptr*/)
	: QWidget(parent)
{
	initWindow();
	connect(m_buttonGroup, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled), this, &SnipToolBar::sigRadioToggled);
	connect(m_closeButton, &QPushButton::clicked, this, &SnipToolBar::sigQuit);
	connect(m_dockShowAnimation, &QPropertyAnimation::finished, this, [this]() {m_bDockShow = true; });
	connect(m_dockHideAnimation, &QPropertyAnimation::finished, this, [this]() {m_bDockShow = false; });
}

void SnipToolBar::showEvent(QShowEvent *event)
{
	dockShow();
	QTimer::singleShot(kDockObjectFirstShowTime, this, &SnipToolBar::dockHide);
}

void SnipToolBar::hideEvent(QHideEvent *event)
{
	dockHide();
}

void SnipToolBar::enterEvent(QEvent* event)
{
	if (QAbstractAnimation::Running == m_dockHideAnimation->state() ||
		QAbstractAnimation::Running == m_dockShowAnimation->state() ||
		m_bDockShow) return;
	dockShow();
}

void SnipToolBar::leaveEvent(QEvent *event)
{
	if (QAbstractAnimation::Running == m_dockHideAnimation->state() ||
		QAbstractAnimation::Running == m_dockShowAnimation->state() ||
		!m_bDockShow) return;
	dockHide();
}

void SnipToolBar::initWindow()
{
	m_buttonGroup = new QButtonGroup(this);
	m_rectButton = new QRadioButton(this);
	m_freeFormButton = new QRadioButton(this);
	m_applicationButton = new QRadioButton(this);
	m_closeButton = new QPushButton(this);

	m_buttonGroup->addButton(m_rectButton, 0);
	m_buttonGroup->addButton(m_freeFormButton, 1);
	m_buttonGroup->addButton(m_applicationButton, 2);
	m_rectButton->setChecked(true);

	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->addWidget(m_rectButton);
	hLayout->addWidget(m_freeFormButton);
	hLayout->addWidget(m_applicationButton);
	hLayout->addWidget(m_closeButton);

	setLayout(hLayout);

	// dock
	m_dockShowAnimation = new QPropertyAnimation(this, "geometry");
	m_dockShowAnimation->setDuration(kDockAnimationDuration);
	m_dockHideAnimation = new QPropertyAnimation(this, "geometry");
	m_dockHideAnimation->setDuration(kDockAnimationDuration);
	QTimer::singleShot(kDockObjectFirstShowTime, this, &SnipToolBar::dockHide);
}

void SnipToolBar::dockShow()
{
	auto end_val = this->geometry();
	end_val.moveTop(end_val.top() - kUnDockTriggerDistance + end_val.height());
	m_dockShowAnimation->setStartValue(this->geometry());
	m_dockShowAnimation->setEndValue(end_val);
	m_dockShowAnimation->start();
}

void SnipToolBar::dockHide()
{
	auto end_val = this->geometry();
	end_val.moveTop(end_val.top() - end_val.height() + kUnDockTriggerDistance);
	m_dockHideAnimation->setStartValue(this->geometry());
	m_dockHideAnimation->setEndValue(end_val);
	m_dockHideAnimation->start();
}
