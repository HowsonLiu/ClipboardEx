#pragma once
#include "DockableWindow.h"
#include "HistoryDataList.h"
#include <QLabel>
#include <QImage>

class QMimeData;
class QCheckBox;
class QListWidget;
class QPropertyAnimation;
class QListWidgetItem;
class QTimer;

/*!
 * \class MimeDataLabel
 * \brief A label show mime data
 * \author liuhaosheng
 * \date October 2019
 */
class MimeDataLabel : public QLabel 
{
	Q_OBJECT
public:
	MimeDataLabel(QWidget* parent = nullptr);
	inline void setMimeData(const ClipboardDataPtr& data) { m_bindMimeData = data; showMimeData(); }
	inline bool isValid() const { return m_bindMimeData ? m_bindMimeData->isValid() : false; }
public slots:
	void onDoubleClicked();
private:
	void showMimeData();
	void showText(const QString& text);
private:
	ClipboardDataPtr m_bindMimeData;
};

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
	void show();

private:
	void initWindow();

private slots:
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
};

