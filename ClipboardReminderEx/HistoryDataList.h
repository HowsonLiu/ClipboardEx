#pragma once
#include <QObject>
#include <QImage>
#include <QString>
#include <deque>
#include <QUrl>

class QMimeData;

/*!
 * \class ClipboardData
 * \brief Actually it's the QMimeData class copy version, but it only saves
the data which i need
 * \author liuhaosheng
 * \date November 2019
 */
struct ClipboardData {
	QImage image;
	QString text;
	QList<QUrl> urls;
	ClipboardData() {};
	ClipboardData(const QMimeData*);
	inline bool hasUrls() const { return !urls.isEmpty(); }
	inline bool hasImage() const { return !image.isNull(); }
	inline bool hasText() const { return !text.isEmpty(); }
};

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
	// older data are in the end of the list
	std::deque<std::shared_ptr<ClipboardData>> m_historyClipboardDataList;
	int m_listSize;
};