#include "assettreewidget.h"

#include <QPainter>
#include <QIcon>
#include <QStyle>
#include <QScrollBar>

AssetTreeWidget::AssetTreeWidget(QWidget* parent)
    : QTreeWidget(parent)
{
}

void AssetTreeWidget::paintEvent(QPaintEvent* event)
{
    QTreeWidget::paintEvent(event);

    QPainter painter(viewport());

    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor("#cbd5e1"));
    pen.setWidth(1);
    painter.setPen(pen);

    for (int i = 0; i < topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* parent = topLevelItem(i);

        if (!parent)
            continue;

        const QRect parentRect =
            visualItemRect(parent);

        if (!parentRect.isValid())
            continue;

        const int branchX =
            viewport()->width() - indentation() / 2;

        // =====================================================
        // CHEVRON
        // =====================================================

        const QString iconPath =
            parent->isExpanded()
                ? ":/qlementine/icons/16/navigation/chevron-down.svg"
                : ":/qlementine/icons/16/navigation/chevron-left.svg";

        QIcon icon(iconPath);

        if (!icon.isNull())
        {
            const int iconSize = 16;

            QRect iconRect(
                branchX - iconSize / 2,
                parentRect.center().y() - iconSize / 2,
                iconSize,
                iconSize
                );

            icon.paint(
                &painter,
                iconRect,
                Qt::AlignCenter
                );
        }

        // =====================================================
        // CONNECTOR LINES
        // =====================================================

        if (!parent->isExpanded())
            continue;

        if (parent->childCount() == 0)
            continue;

        QTreeWidgetItem* lastChild = nullptr;

        for (int c = 0; c < parent->childCount(); ++c)
        {
            QTreeWidgetItem* child =
                parent->child(c);

            if (!child->isHidden())
                lastChild = child;
        }

        if (!lastChild)
            continue;

        const QRect lastChildRect =
            visualItemRect(lastChild);

        if (!lastChildRect.isValid())
            continue;

        // Vertical line.
        painter.drawLine(
            branchX,
            parentRect.bottom(),
            branchX,
            lastChildRect.center().y()
            );

        // Horizontal line for every child.
        for (int c = 0; c < parent->childCount(); ++c)
        {
            QTreeWidgetItem* child =
                parent->child(c);

            if (child->isHidden())
                continue;

            const QRect childRect =
                visualItemRect(child);

            if (!childRect.isValid())
                continue;

            const int childY =
                childRect.center().y();

            painter.drawLine(
                branchX,
                childY,
                branchX - indentation(),
                childY
                );
        }
    }
}
void AssetTreeWidget::drawItemBranch(
    QPainter& painter,
    QTreeWidgetItem* item
    )
{
    const QRect itemRect = visualItemRect(item);

    if (!itemRect.isValid())
        return;

    // For RTL tree, branch area is on the right.
    const int branchX =
        viewport()->width() - indentation() / 2;

    const int centerY =
        itemRect.center().y();

    // =========================
    // VERTICAL CONNECTOR
    // =========================

    if (item->isExpanded())
    {
        painter.drawLine(
            branchX,
            centerY,
            branchX,
            itemRect.bottom() +
                item->childCount() * 40
            );
    }

    // =========================
    // HORIZONTAL CONNECTOR
    // =========================

    painter.drawLine(
        branchX - indentation(),
        centerY,
        branchX,
        centerY
        );

    // =========================
    // CHEVRON
    // =========================

    const QString iconPath =
        item->isExpanded()
            ? ":/qlementine/icons/16/navigation/chevron-down.svg"
            : ":/qlementine/icons/16/navigation/chevron-right.svg";

    QIcon icon(iconPath);

    if (!icon.isNull())
    {
        const int size = 16;

        QRect iconRect(
            branchX - size / 2,
            centerY - size / 2,
            size,
            size
            );

        icon.paint(
            &painter,
            iconRect,
            Qt::AlignCenter
            );
    }
}