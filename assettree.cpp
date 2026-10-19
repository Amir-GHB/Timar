#include "assettree.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPalette>

AssetTree::AssetTree(QWidget* parent)
    : QTreeWidget(parent)
{
    setAutoFillBackground(false);
    viewport()->setAutoFillBackground(false);
}

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

void AssetTree::drawRow(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
    ) const
{
    QStyleOptionViewItem customOption(option);
    customOption.backgroundBrush = QColor("#e3e3e3");

    QTreeWidget::drawRow(painter, customOption, index);
}