#ifndef ASSETTREEWIDGET_H
#define ASSETTREEWIDGET_H

#include <QTreeWidget>

class AssetTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit AssetTreeWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawBranch(
        QPainter& painter,
        QTreeWidgetItem* item,
        int depth
        );

    void drawItemBranch(
        QPainter& painter,
        QTreeWidgetItem* item
        );
};

#endif