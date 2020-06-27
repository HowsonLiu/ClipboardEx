#include "floatlayout.h"
#include <QWidget>

FloatLayout::FloatLayout(QWidget *parent)
	: QLayout(parent)
	, m_underWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);
}

FloatLayout::~FloatLayout()
{
	while (!m_itemList.empty())
		delete m_itemList.takeFirst();
}

QSize FloatLayout::sizeHint() const
{
	return m_underWidget->sizeHint();
}

QSize FloatLayout::minimumSize() const
{
	return m_underWidget->minimumSize();
}

void FloatLayout::addItem(QLayoutItem *item)
{
	m_itemList.push_back(item);
	m_marginList.push_back(QRect());
}

QLayoutItem* FloatLayout::itemAt(int index) const
{
	return index >= 0 && index < m_itemList.count() ?
		m_itemList.at(index) : nullptr;
}

QLayoutItem* FloatLayout::takeAt(int index)
{
	if (index < 0 || index >= m_itemList.count()) return nullptr;
	m_marginList.takeAt(index);
	return m_itemList.takeAt(index);
}

int FloatLayout::count() const
{
	return m_itemList.count();
}

bool FloatLayout::isEmpty() const
{
	return m_itemList.isEmpty();
}

void FloatLayout::setGeometry(const QRect& rect)
{
	QLayout::setGeometry(rect);
	if (m_itemList.empty()) return;

	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);

	m_underWidget->setGeometry(effectiveRect);

	for (int i = 0; i < m_itemList.size(); ++i) {
		QRect margin = m_marginList.at(i);
		QRect itemRect = effectiveRect.adjusted(margin.left(), margin.top(), -margin.right(), -margin.bottom());

		QLayoutItem *item = m_itemList.at(i);
		QSize size = item->sizeHint();
		auto align = item->alignment();
		if (align & Qt::AlignLeft) {
			itemRect.setWidth(size.width());
		}
		else if (align & Qt::AlignRight) {
			itemRect.setLeft(itemRect.right() - size.width());
		}
		else if (align & Qt::AlignHCenter) {
			itemRect.setLeft(itemRect.left() + (itemRect.width() - size.width()) / 2);
			itemRect.setWidth(size.width());
		}

		if (align & Qt::AlignTop) {
			itemRect.setHeight(size.height());
		}
		else if (align & Qt::AlignBottom) {
			itemRect.setTop(itemRect.bottom() - size.height());
		}
		else if (align & Qt::AlignHCenter) {
			itemRect.setTop(itemRect.top() + (itemRect.height() - size.height()) / 2);
			itemRect.setHeight(size.height());
		}

		item->setGeometry(itemRect);
	}
}

void FloatLayout::addWidget(QWidget *w,
	Qt::Alignment align /*= Qt::AlignTop | Qt::AlignHCenter*/,
	int marginLeft /*= 0*/,
	int marginTop /*= 0*/,
	int marginRight /*= 0*/,
	int marginBottom /*= 0*/)
{
	if (Q_UNLIKELY(!w)) return;
	if (Q_UNLIKELY(w == m_underWidget)) return;
	if (Q_UNLIKELY(w == parentWidget())) return;

	addChildWidget(w);

	QWidgetItemV2 *item = new QWidgetItemV2(w);
	item->setAlignment(align);
	m_itemList.push_back(item);
	m_marginList.push_back(QRect(marginLeft, marginTop, marginRight - marginLeft, marginBottom - marginTop));
}
