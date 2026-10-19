#ifndef MAINTENANCESCHEDULE_H
#define MAINTENANCESCHEDULE_H

//#include "maintenanceevent.h"
#include <QDateTime>
#include <QString>
#include <QUuid>

enum class MaintenanceIntervalType
{
    Days,
    Weeks,
    Months,
    Years
};

enum class MaintenanceScheduleStatus
{
    Active,
    Paused
};

struct MaintenanceSchedule
{
    QUuid id;

    QUuid assetId;
    QUuid subAssetId;

    QString title;

    int interval = 0;

    MaintenanceIntervalType intervalType =
        MaintenanceIntervalType::Days;

    QDate nextScheduledDate;

    MaintenanceScheduleStatus status =
        MaintenanceScheduleStatus::Active;

    //QList<MaintenancePart> plannedParts;
};

#endif // MAINTENANCESCHEDULE_H