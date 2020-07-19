#pragma once
#include <QWidget>

class QButtonGroup;
class QRadioButton;
class QPushButton;
class QPropertyAnimation;

/** @class SnipToolBar
  * @brief SnipWindow's toolbar, select snip mode.
  * @copyright (c) 2020, HowsonLiu. All rights reserved
  * @author HowsonLiu
  * @date 2020/6/27
  */
class SnipToolBar : public QWidget
{
	Q_OBJECT
public:
    SnipToolBar(QWidget* parent = nullptr);

protected:
	virtual void showEvent(QShowEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent *event) override;

signals:
	void sigRadioToggled(int, bool);
	void sigQuit();

private:
	void initWindow();

private slots:
	void dockShow();
	void dockHide();

private:
	QButtonGroup* m_buttonGroup;
	QRadioButton* m_rectButton;
	QRadioButton* m_freeFormButton;
	QRadioButton* m_applicationButton;
	QPushButton* m_closeButton;

	// dock
	bool m_bDockShow = true;
	QPropertyAnimation* m_dockShowAnimation = nullptr;
	QPropertyAnimation* m_dockHideAnimation = nullptr;
};