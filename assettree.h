#pragma once

#include <QTreeWidget>

    class AssetTree : public QTreeWidget
{
    Q_OBJECT

public:
    explicit AssetTree(QWidget* parent = nullptr);

protected:


void drawRow(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
    ) const override;
};