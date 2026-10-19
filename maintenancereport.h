#pragma once

#include <QDate>
#include <QList>

#include "maintenanceevent.h"

struct MaintenanceReport
{
    QDate fromDate;
    QDate toDate;

    QList<MaintenanceEventData> events;

    int eventCount() const
    {
        return events.size();
    }
};