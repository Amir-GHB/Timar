#pragma once

#include "maintenancereport.h"
#include "maintenancereporttypes.h"

class MaintenanceManager;

class MaintenanceReportGenerator
{
public:
    explicit MaintenanceReportGenerator(
        const MaintenanceManager& maintenanceManager
        );

    MaintenanceReport generate(
        const MaintenanceEventFilter& filter
        ) const;

private:
    const MaintenanceManager& m_maintenanceManager;
};