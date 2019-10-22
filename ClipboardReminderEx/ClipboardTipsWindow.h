#pragma once
#include "DockableWindow.h"

class QMimeData;

class HistoryDataList : public QObject 
{
	Q_OBJECT
public:
	static HistoryDataList* getInstance();

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

/*!
 * \class ClipboardTipsWindow
 *
 * \brief Show clipboard current and prevent content.
 *
 * \author HowsonLiu
 * \date October 2019
 */
class ClipboardTipsWindow : public DockableWindow
{
	Q_OBJECT
public:
	ClipboardTipsWindow(QWidget* parent = nullptr);

private:
	void initWindow();
};

