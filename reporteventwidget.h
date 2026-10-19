#ifndef REPORTEVENTWIDGET_H
#define REPORTEVENTWIDGET_H

#include "maintenanceevent.h"

#include <QWidget>

class QLabel;
class QPushButton;
class QFrame;

class ReportEventWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportEventWidget(
        const MaintenanceEventData& eventData,
        QWidget* parent = nullptr
        );

signals:
    void detailsRequested(
        const QUuid& eventId
        );

private:
    void setupUI();
    void updateUI();
    void updateStatus();

    QWidget* createInfoItem(
        const QString& iconPath,
        const QString& title,
        QLabel*& valueLabel
        );

    QFrame* createInfoSeparator(
        QWidget* parent
        );

    MaintenanceEventData m_eventData;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_typeLabel = nullptr;
    QLabel* m_placeLabel = nullptr;
    QLabel* m_dateLabel = nullptr;

    QLabel* m_statusLabel = nullptr;

    QPushButton* m_detailsButton = nullptr;
};

#endif // REPORTEVENTWIDGET_H