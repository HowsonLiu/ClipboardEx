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
	inline bool hasUrls() const { return !urls.isEmpty(); }
	inline bool hasImage() const { return !image.isNull(); }
	inline bool hasText() const { return !text.isEmpty(); }
	inline bool isValid() const { return hasUrls() || hasImage() || hasText(); }
	void clear();
	void copyFromClipboard();
	void copyToClipboard();
};
typedef std::shared_ptr<ClipboardData> ClipboardDataPtr;
Q_DECLARE_METATYPE(ClipboardDataPtr);

/*!
 * \class ReadClipboardThread
 * \brief In some case, we can not read the clipboard data at once. So I took this operation
 on another thread
 * \author liuhaosheng
 * \date December 2019
 */
class ReadClipboardThread : public QThread 
{
	Q_OBJECT
public:
	explicit ReadClipboardThread(QObject* parent = nullptr) { qRegisterMetaType<ClipboardDataPtr>(); };
protected:
	virtual void run() override;
signals:
	void sigSuccessed(const ClipboardDataPtr& data);
	void sigFailed();
};

class HistoryDataList : public QObject
{
	Q_OBJECT
public:
	static HistoryDataList* getInstance();
	inline auto dataList() const { return &m_historyClipboardDataList; };
	inline auto size() const { return m_historyClipboardDataList.size(); };
	inline auto capacity() const { return m_listCapacity; }
	void init() { m_thread->start(); };
private:
	HistoryDataList(QObject* parent = nullptr);
signals:
	// called when resize the list or clipboard update
	void sigDataListUpdate();
public slots:
	void onSetListSize(int);
private slots:
	void onReadSuccessed(const ClipboardDataPtr& data);
	void onReadFailed();
private:
	ReadClipboardThread* m_thread;
	// older data are in the end of the list
	std::deque<ClipboardDataPtr> m_historyClipboardDataList;
	int m_listCapacity;
};