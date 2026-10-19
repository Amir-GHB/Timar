#ifndef MAINTENANCEEVENT_H
#define MAINTENANCEEVENT_H

#include <QDateTime>
#include <QString>
#include <QUuid>

enum class MaintenanceType
{
    PM,
    EM
};

enum class MaintenanceStatus
{
    Scheduled,
    InProgress,
    WaitingForHandling,
    Completed,
    Cancelled,
    Overdue,
    Missed
};
/*
struct MaintenancePart
{
    QUuid storagePartId;
    QString partName;
    QString partNumber;
    int plannedQuantity = 0;
    int usedQuantity = 0;
};
*/

struct MaintenanceEventData
{
    QUuid id;

    QString title;
    QString description;

    QString assetName;
    QString subAssetName;
    QString installationPlace;

    QString facilitySection;

    MaintenanceType type = MaintenanceType::PM;

    QDate scheduledDate;
    QDateTime completedTime;

    MaintenanceStatus status =
        MaintenanceStatus::Scheduled;

    QUuid assetId;
    QUuid subAssetId;
    QUuid scheduleId;

    QString technician;

    QList<QUuid> affectedScheduleIds;

    //QList<MaintenancePart> parts;
};

class MaintenanceEvent
{
public:
    MaintenanceEvent();

    explicit MaintenanceEvent(
        const MaintenanceEventData& eventData
    );

    MaintenanceEventData& data();
    const MaintenanceEventData& data() const;

    void setStatus(MaintenanceStatus status);

    bool isScheduledEvent() const;
    bool isUnplannedEvent() const;
private:
    MaintenanceEventData m_data;


};

#endif // MAINTENANCEEVENT_H