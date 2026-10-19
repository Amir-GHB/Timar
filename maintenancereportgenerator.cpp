#include "maintenancereportgenerator.h"

#include "maintenancemanager.h"

MaintenanceReportGenerator::MaintenanceReportGenerator(
    const MaintenanceManager& maintenanceManager
    )
    : m_maintenanceManager(maintenanceManager)
{
}

MaintenanceReport MaintenanceReportGenerator::generate(
    const MaintenanceEventFilter& filter
    ) const
{
    MaintenanceReport report;

    report.fromDate = filter.fromDate;
    report.toDate = filter.toDate;

    report.events =
        m_maintenanceManager.eventsForFilter(filter);

    return report;
}