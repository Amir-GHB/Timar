#pragma once

#include <QWidget>
#include <QPainter>
#include <QColor>

class ReportCardWidget : public QWidget
{
public:
    explicit ReportCardWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_StyledBackground, false);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QRectF rect = this->rect();
        rect.adjust(0.5, 0.5, -0.5, -0.5);

        painter.setPen(QPen(QColor("#e2e8f0"), 1));
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(rect, 12, 12);
    }
};