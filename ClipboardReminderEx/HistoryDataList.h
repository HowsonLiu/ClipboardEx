#pragma once
#include <QObject>

class QMimeData;
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
	QList<QMimeData*> m_historyClipboardDataList;
	int m_listSize;
};
QMimeData* deepCopyMimeData(const QMimeData*);
