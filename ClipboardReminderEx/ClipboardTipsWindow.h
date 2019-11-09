#pragma once
#include "DockableWindow.h"
#include <QLabel>

class QMimeData;
class QCheckBox;
class QListWidget;
class QPropertyAnimation;

class HistoryDataList : public QObject 
{
	Q_OBJECT
public:
	static HistoryDataList* getInstance();
	inline auto dataList() const { return &m_historyClipboardDataList; }

private:
	HistoryDataList(QObject* parent = nullptr);
	~HistoryDataList();

signals:
	// called when resize the list or clipboard update
	void sigDataListUpdate();

public slots:
	void onSetListSize(int);

private slots:
	void onClipboardDataUpdate();

private:
	// older data are in the front of the list
	QList<const QMimeData*> m_historyClipboardDataList;
	int m_listSize;
};
QMimeData* deepCopyMimeData(const QMimeData*);

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
	void showMimeData(const QMimeData*);

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
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
	void resizeLabel(const QSize&);

private:
	void initWindow();
	void beautyWindow();

private slots:
	void onHistoryListUpdate();
	void onCheckBoxStateChanged(int state);

private:
	MimeDataLabel* m_curMimeDataLabel;
	QListWidget* m_historyMimeDataListWidget;
	QCheckBox* m_expandCheckBox;
	QCheckBox* m_autoShowCheckBox;
	QPropertyAnimation* m_expandAnimation;
	QPropertyAnimation* m_shrinkAnimation;
};

