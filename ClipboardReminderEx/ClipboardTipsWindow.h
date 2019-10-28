#pragma once
#include "DockableWindow.h"
#include <QLabel>

class QMimeData;
class QPushButton;
class QListWidget;

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

private:
	void initWindow();

private:
	MimeDataLabel* m_curMimeDataLabel;
	QListWidget* m_historyMimeDataListWidget;
	QPushButton* m_expandButton;
};

