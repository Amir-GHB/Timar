#ifndef CUSTOMICONS_H
#define CUSTOMICONS_H

#include <qicon.h>
#include <qpainter.h>
#include <qwidget.h>

class RotatableChevron : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal angle READ angle WRITE setAngle)

public:
    explicit RotatableChevron(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(14, 14);
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet("background: transparent;");
    }

    qreal angle() const
    {
        return m_angle;
    }

    void setAngle(qreal angle)
    {
        m_angle = angle;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);

        painter.setRenderHint(
            QPainter::Antialiasing,
            true
            );

        painter.setCompositionMode(
            QPainter::CompositionMode_SourceOver
            );

        QPixmap pixmap =
            QIcon(
                ":/qlementine/icons/16/navigation/chevron-down.svg"
                ).pixmap(12, 12);

        painter.translate(
            width() / 2.0,
            height() / 2.0
            );

        painter.rotate(m_angle);

        painter.drawPixmap(
            -pixmap.width() / 2,
            -pixmap.height() / 2,
            pixmap
            );
    }

private:
    qreal m_angle = 0.0;
};
#endif // CUSTOMICONS_H