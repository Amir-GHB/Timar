#ifndef MAINTENANCESCHEDULEWIDGET_H
#define MAINTENANCESCHEDULEWIDGET_H

#include "customicons.h"
#include "maintenanceschedule.h"

#include <QFrame>
#include <qpushbutton.h>
#include <QPropertyAnimation>
class QLabel;
class MaintenanceScheduleWidget : public QFrame
{
    Q_OBJECT

public:
    explicit MaintenanceScheduleWidget(
        const MaintenanceSchedule& schedule,
        QWidget* parent = nullptr
        );

signals:
    void statusChangeRequested(
        const QUuid& scheduleId,
        MaintenanceScheduleStatus status
        );

    void removeRequested(
        const QUuid& scheduleId
        );

private:
    void setupUI();
    QString intervalText() const;

    void setupStatusControl();
    void updateStatusControl();

private:
    MaintenanceSchedule m_schedule;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_assetLabel = nullptr;
    QLabel* m_nextScheduleLabel = nullptr;
    QLabel* m_intervalLabel = nullptr;


    QPushButton* m_statusButton = nullptr;
    QFrame* m_statusMenu = nullptr;

    QPushButton* m_pauseResumeButton = nullptr;
    QPushButton* m_removeButton = nullptr;
    QLabel* m_pauseResumeTitle = nullptr;
    QLabel* m_pauseResumeDescription = nullptr;

    QLabel* m_removeTitle = nullptr;
    QLabel* m_removeDescription = nullptr;

    QFrame* m_pauseResumeIconFrame = nullptr;
    QFrame* m_removeIconFrame = nullptr;
    QLabel* pauseIcon = nullptr;

    RotatableChevron* m_statusChevron = nullptr;
    QPropertyAnimation* m_chevronAnimation = nullptr;
    QLabel* m_statusText = nullptr;
    bool m_ignoreStatusButtonRelease = false;

protected:
    bool eventFilter(
        QObject* watched,
        QEvent* event
        ) override;


};


#endif