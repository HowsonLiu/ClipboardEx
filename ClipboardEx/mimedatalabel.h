#pragma once
#include <QLabel>
#include "HistoryDataList.h"

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
	QRect textRect();
	void showMimeData();
	void showText(const QString& text);
private:
	ClipboardDataPtr m_bindMimeData;
};