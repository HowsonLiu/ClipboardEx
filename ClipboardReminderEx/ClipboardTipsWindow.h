#pragma once
#include "DockableWindow.h"
#include "HistoryDataList.h"
#include <QLabel>
#include <QImage>

class QMimeData;
class QCheckBox;
class QListWidget;
class QPropertyAnimation;

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
	inline void setMimeData(const std::shared_ptr<ClipboardData> data) { m_bindMimeData = data; showMimeData(); }
	inline void setDisplay(bool b) { m_bDisplay = b; }

public slots:
	void onUpdateSize(const QSize&);

private:
	void showMimeData();

private:
	std::shared_ptr<ClipboardData> m_bindMimeData;
	bool m_bDisplay;
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
	void setLabelSize(const QSize&);
	void setListHeight(int);

private:
	void initWindow();
	void beautyWindow();

signals:
	void sigUpdateLabelSize(const QSize&);

private slots:
	void onHistoryListUpdate();
	void onExpandStateChanged(int state);
	void onItemDoubleClicked(QListWidgetItem*);

private:
	MimeDataLabel* m_curMimeDataLabel;
	QListWidget* m_historyMimeDataListWidget;
	QCheckBox* m_expandCheckBox;
	QCheckBox* m_autoShowCheckBox;

	int m_listHeight;
};

