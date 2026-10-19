#include "maintenanceevent.h"

MaintenanceEvent::MaintenanceEvent()
{
    m_data.id = QUuid::createUuid();
}

MaintenanceEvent::MaintenanceEvent(
    const MaintenanceEventData& eventData
)
    : m_data(eventData)
{
    if (m_data.id.isNull())
        m_data.id = QUuid::createUuid();
}

MaintenanceEventData& MaintenanceEvent::data()
{
    return m_data;
}

const MaintenanceEventData& MaintenanceEvent::data() const
{
    return m_data;
}

void MaintenanceEvent::setStatus(MaintenanceStatus status)
{
    m_data.status = status;

    if (status == MaintenanceStatus::Completed)
    {
        m_data.completedTime = QDateTime::currentDateTime();
    }
}

bool MaintenanceEvent::isScheduledEvent() const
{
    return !m_data.scheduleId.isNull();
}

bool MaintenanceEvent::isUnplannedEvent() const
{
    return m_data.scheduleId.isNull();
}