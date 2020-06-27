#pragma once
#include "util/DockableWindow.h"

class QCheckBox;
class QListWidget;
class QListWidgetItem;
class QTimer;
class MimeDataLabel;

/*!
 * \class ClipboardTipsWindowState
 * \brief ClipboardTipsWindow's state serializatior
 * \author liuhaosheng
 * \date November 2019
 */
struct ClipboardTipsWindowState {
	bool bExpand;
	bool bAutoShow;
	DockableWindowState dockState;
public:
	ClipboardTipsWindowState();
	ClipboardTipsWindowState(const QString&);
	operator QString();
};

/*!
 * \class ClipboardTipsWindow
 * \brief Show clipboard current and prevent content.
 * \author HowsonLiu
 * \date October 2019
 */
class ClipboardTipsWindow : public DockableWindow
{
	Q_OBJECT
public:
	ClipboardTipsWindow(QWidget* parent = nullptr);
	ClipboardTipsWindowState getTipsWindowState() const;

	void loadTipsWindowState(const ClipboardTipsWindowState&);
	void updateHistoryList();

private:
	void initWindow();
	void startHide();
	void stopHide();

private slots:
	void onContentMenuShow(bool);
	void onHistoryListUpdate();
	void onExpandStateChanged(int state);
	void onItemDoubleClicked(QListWidgetItem*);

protected:
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual bool eventFilter(QObject *watched, QEvent *event);

private:
	MimeDataLabel* m_curMimeDataLabel;
	QListWidget* m_historyMimeDataListWidget;
	QCheckBox* m_expandCheckBox;
	QCheckBox* m_autoShowCheckBox;

	QTimer* m_timer;
	QTimer* m_dockTimer;
	
	bool m_showingContextMenu;
};

