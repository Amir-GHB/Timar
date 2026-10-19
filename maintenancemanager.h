#ifndef MAINTENANCEMANAGER_H
#define MAINTENANCEMANAGER_H

#include "assetmanager.h"
#include "maintenanceevent.h"
#include "maintenanceschedule.h"
#include "maintenancereporttypes.h"
#include "storagemanager.h"

#include <QVector>
#include <QUuid>
#include <QTimer>
#include <QObject>

class MaintenanceManager : public QObject
{
    Q_OBJECT

public:
    explicit MaintenanceManager( AssetManager& assetManager, QObject* parent = nullptr);

    AssetManager& m_assetManager;

    // Events
    MaintenanceEvent* addEvent(const MaintenanceEventData& data);

    MaintenanceEvent* getEvent(const QUuid& id);

    const QVector<MaintenanceEvent>& events() const;

    QList<MaintenanceEventData> eventsForDate(
        const QDate& date
        ) const;

    bool removeEvent(const QUuid& id);

    bool setEventStatus(const QUuid& id,MaintenanceStatus status);

    // Schedules
    MaintenanceSchedule* addSchedule(const MaintenanceSchedule& schedule);

    MaintenanceSchedule* getSchedule(const QUuid& id);

    const QVector<MaintenanceSchedule>& schedules() const;

    bool removeSchedule(const QUuid& id);

    bool setScheduleStatus(
        const QUuid& id,
        MaintenanceScheduleStatus status
        );

    bool completeEvent(
        const QUuid& eventId,
        const QDate& completionDate
        );

    QDate nextScheduledEventDate() const;

    bool removeAssetData(const QUuid& assetId);

    void clearData();

    void restoreEvent(
        const MaintenanceEvent& event
        );

    void restoreSchedule(
        const MaintenanceSchedule& schedule
        );

    bool removeSubAssetData(
        const QUuid& assetId,
        const QUuid& subAssetId
        );

    QList<MaintenanceEventData> eventsForFilter(
        const MaintenanceEventFilter& filter
        ) const;

signals:
    void eventsUpdated();
    void schedulesUpdated();

private slots:
    void checkEventStatuses();
    void processSchedules();

private:
    void advanceSchedule(MaintenanceSchedule& schedule);

    MaintenanceEvent* findEventForScheduleDate(
        const QUuid& scheduleId,
        const QDate &scheduledDate
        );

    QVector<MaintenanceEvent> m_events;
    QVector<MaintenanceSchedule> m_schedules;

    QTimer* m_statusTimer = nullptr;

    void removePendingEventForSchedule(
        const QUuid& scheduleId,
        const QDate& scheduledDate
        );

    QDate calculateNextScheduledDate(
        const MaintenanceSchedule& schedule,
        const QDate& fromDate
        ) const;

};

#endif // MAINTENANCEMANAGER_H