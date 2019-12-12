#pragma once
#include <QObject>
#include <QImage>
#include <QString>
#include <deque>
#include <QUrl>
#include <QThread>

class QMimeData;
class QTimer;

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
	inline bool isValid() const { return hasUrls() || hasImage() || hasText(); }
	void clear();
};

class ReadClipboardThread : public QThread 
{
	Q_OBJECT
public:
	explicit ReadClipboardThread(QObject* parent = nullptr) {};
protected:
	virtual void run() override;
signals:
	void sigSuccessed(const ClipboardData& data);
	void sigFailed();
private:
	ClipboardData m_data;
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
	void onClipboardDataUpdate(const ClipboardData& data);
private:
	// older data are in the end of the list
	std::deque<std::shared_ptr<ClipboardData>> m_historyClipboardDataList;
	int m_listSize;
	ReadClipboardThread* m_thread;
};