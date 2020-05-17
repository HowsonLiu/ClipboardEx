#pragma once
#include <QStyledItemDelegate>

/** @class NoFocusDelegate
  * @brief fix list widget focus rectangle
  * https://blog.csdn.net/love_gaohz/article/details/13019841
  * @author HowsonLiu
  * @date 2020/5/17
  */
class NoFocusDelegate :public QStyledItemDelegate
{
	void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const;
};