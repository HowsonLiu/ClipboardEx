#pragma once
#include <QLayout>
#include <QList>

class FloatLayout : public QLayout
{
public:
	FloatLayout(QWidget* parent);
	~FloatLayout();

	virtual QSize sizeHint() const override;
	virtual QSize minimumSize() const override;
	virtual void addItem(QLayoutItem*) override;
	virtual QLayoutItem* itemAt(int index) const override;
	virtual QLayoutItem* takeAt(int index) override;
	virtual int count() const override;
	virtual bool isEmpty() const override;
	virtual void setGeometry(const QRect&) override;

	void addWidget(QWidget *w, 
		Qt::Alignment align = Qt::AlignTop | Qt::AlignHCenter, 
		int marginLeft = 0, 
		int marginTop = 0, 
		int marginRight = 0, 
		int marginBottom = 0);

private:
	QWidget* m_underWidget;
	QList<QLayoutItem*> m_itemList;
	QList<QRect> m_marginList;
};

