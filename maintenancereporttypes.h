#pragma once

#include <QDate>
#include <QList>
#include <QUuid>

#include "maintenanceevent.h"

struct MaintenanceEventFilter
{
    QDate fromDate;
    QDate toDate;

    QList<QUuid> assetIds;
    QList<QUuid> subAssetIds;

    QList<MaintenanceType> types;
    QList<MaintenanceStatus> statuses;

    bool assetFilterEnabled = false;
};