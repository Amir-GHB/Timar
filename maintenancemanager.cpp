#include "maintenancemanager.h"
#include "asset.h"

MaintenanceManager::MaintenanceManager(
    AssetManager& assetManager,
    QObject* parent)
    : QObject(parent),
    m_assetManager(assetManager)
{
    m_statusTimer = new QTimer(this);

    connect(
        m_statusTimer,
        &QTimer::timeout,
        this,
        &MaintenanceManager::checkEventStatuses
        );

    connect(
        m_statusTimer,
        &QTimer::timeout,
        this,
        &MaintenanceManager::processSchedules
        );

    checkEventStatuses();
    processSchedules();

    m_statusTimer->start(60 * 1000);
}

MaintenanceEvent* MaintenanceManager::addEvent(
    const MaintenanceEventData& data
    )
{
    MaintenanceEvent event(data);

    m_events.append(event);

    emit eventsUpdated();

    return &m_events.last();
}
MaintenanceEvent* MaintenanceManager::getEvent(
    const QUuid& id
    )
{
    for (auto& event : m_events)
    {
        if (event.data().id == id)
            return &event;
    }

    return nullptr;
}

const QVector<MaintenanceEvent>&
MaintenanceManager::events() const
{
    return m_events;
}

bool MaintenanceManager::removeEvent(
    const QUuid& id
    )
{
    for (int i = 0; i < m_events.size(); ++i)
    {
        if (m_events[i].data().id != id)
            continue;

        m_events.removeAt(i);

        emit eventsUpdated();

        return true;
    }

    return false;
}
bool MaintenanceManager::setEventStatus(
    const QUuid& id,
    MaintenanceStatus status
    )
{
    MaintenanceEvent* event = getEvent(id);

    if (!event)
        return false;

    event->setStatus(status);

    emit eventsUpdated();

    return true;
}

void MaintenanceManager::checkEventStatuses()
{
    const QDateTime now = QDateTime::currentDateTime();

    bool changed = false;

    for (MaintenanceEvent& event : m_events)
    {
        MaintenanceEventData& data = event.data();

      /*  qDebug()
            << "CHECK EVENT:"
            << data.id
            << "status =" << static_cast<int>(data.status)
            << "scheduled =" << data.scheduledDate
            << "now =" << now;*/

        if (data.status != MaintenanceStatus::Scheduled)
            continue;

        // No deadline → nothing to check.
        if (!data.scheduledDate.isValid())
            continue;

        if (data.scheduledDate < now.date())
        {
            data.status = MaintenanceStatus::Overdue;
            changed = true;
        }
    }

    if (changed)
        emit eventsUpdated();
}


MaintenanceSchedule* MaintenanceManager::addSchedule(const MaintenanceSchedule& schedule)
{
    m_schedules.append(schedule);


    MaintenanceSchedule& addedSchedule = m_schedules.last();

    if (addedSchedule.status == MaintenanceScheduleStatus::Active)
        processSchedules();

    emit schedulesUpdated();

    return &m_schedules.last();
}

MaintenanceSchedule* MaintenanceManager::getSchedule(const QUuid& id)
{
    for (auto& schedule : m_schedules)
    {
        if (schedule.id == id)
            return &schedule;
    }

    return nullptr;
}

const QVector<MaintenanceSchedule>&MaintenanceManager::schedules() const
{
    return m_schedules;
}

bool MaintenanceManager::removeSchedule(
    const QUuid& id
    )
{
    MaintenanceSchedule* schedule =
        getSchedule(id);

    if (!schedule)
        return false;

    // Remove pending events belonging to this schedule.
    for (int i = m_events.size() - 1; i >= 0; --i)
    {
        const MaintenanceEventData& data =
            m_events[i].data();

        if (data.scheduleId != id)
            continue;

        if (data.status != MaintenanceStatus::Scheduled &&
            data.status != MaintenanceStatus::Overdue)
        {
            continue;
        }

        m_events.removeAt(i);
    }

    // Remove the schedule itself.
    for (int i = 0; i < m_schedules.size(); ++i)
    {
        if (m_schedules[i].id == id)
        {
            m_schedules.removeAt(i);
            break;
        }
    }

    emit eventsUpdated();
    emit schedulesUpdated();

    return true;
}

void MaintenanceManager::processSchedules()
{
    const QDate today = QDate::currentDate();

    bool eventsChanged = false;
    bool schedulesChanged = false;

    for (MaintenanceSchedule& schedule : m_schedules)
    {
        if (schedule.status != MaintenanceScheduleStatus::Active ||
            !schedule.nextScheduledDate.isValid() ||
            schedule.interval <= 0)
        {
            continue;
        }

        while (schedule.nextScheduledDate <= today)
        {
            MaintenanceEvent* event =
                findEventForScheduleDate(
                    schedule.id,
                    schedule.nextScheduledDate
                    );

            // No event exists for this occurrence.
            // Create it.
            if (!event)
            {
                MaintenanceEventData data;

                data.id = QUuid::createUuid();
                data.title = schedule.title;

                data.assetId = schedule.assetId;
                data.subAssetId = schedule.subAssetId;

                //data.parts = schedule.plannedParts;

                if (Asset* asset = m_assetManager.getAsset(schedule.assetId))
                {
                    data.assetName = asset->info().name;

                    if (!schedule.subAssetId.isNull())
                    {
                        for (const SubAsset& subAsset : asset->subAssets())
                        {
                            if (subAsset.id() == schedule.subAssetId)
                            {
                                data.subAssetName =
                                    subAsset.info().name;

                                break;
                            }
                        }
                    }
                }

                data.scheduledDate = schedule.nextScheduledDate;
                data.status = MaintenanceStatus::Scheduled;
                data.type = MaintenanceType::PM;
                data.scheduleId = schedule.id;

                m_events.append(MaintenanceEvent(data));

                eventsChanged = true;

                break;
            }

            MaintenanceStatus status = event->data().status;

         /*   qDebug()
                << "PROCESS SCHEDULE:"
                << schedule.id
                << "event =" << event->data().id
                << "status =" << static_cast<int>(status)
                << "next =" << schedule.nextScheduledDate
                << "now =" << today;
*/
            // The previous occurrence was not completed before
            // the next occurrence arrived.
            if (status == MaintenanceStatus::Overdue)
            {
                MaintenanceSchedule nextSchedule = schedule;

                advanceSchedule(nextSchedule);

                if (nextSchedule.nextScheduledDate > today)
                {
                    // The next occurrence has not arrived yet.
                    // Keep the current event overdue.
                    break;
                }

                // The next occurrence has arrived.
                // The previous event is now considered missed.
                event->setStatus(MaintenanceStatus::Missed);

                eventsChanged = true;

                advanceSchedule(schedule);

                schedulesChanged = true;

                continue;
            }

            // These occurrences are already resolved.
            // Move the schedule forward.
            if (status == MaintenanceStatus::Completed ||
                status == MaintenanceStatus::Cancelled ||
                status == MaintenanceStatus::Missed)
            {
                advanceSchedule(schedule);

                schedulesChanged = true;

                continue;
            }

            // Scheduled or InProgress.
            // Do not advance the schedule.
            break;
        }
    }

    if (eventsChanged)
        emit eventsUpdated();

    if (schedulesChanged)
        emit schedulesUpdated();
}
void MaintenanceManager::advanceSchedule(
    MaintenanceSchedule& schedule
    )
{
    schedule.nextScheduledDate =
        calculateNextScheduledDate(
            schedule,
            schedule.nextScheduledDate
            );
}
MaintenanceEvent*
MaintenanceManager::findEventForScheduleDate(
    const QUuid& scheduleId,
    const QDate& scheduledDate
    )
{
    for (auto& event : m_events)
    {
        const MaintenanceEventData& data =
            event.data();

        if (data.scheduleId != scheduleId)
            continue;

        if (data.scheduledDate != scheduledDate)
            continue;

        return &event;
    }

    return nullptr;
}

bool MaintenanceManager::setScheduleStatus(
    const QUuid& id,
    MaintenanceScheduleStatus status
    )
{
    MaintenanceSchedule* schedule =
        getSchedule(id);

    if (!schedule)
        return false;

    if (schedule->status == status)
        return true;

    schedule->status = status;

    if (status == MaintenanceScheduleStatus::Paused)
    {
        removePendingEventForSchedule(
            schedule->id,
            schedule->nextScheduledDate
            );

        emit eventsUpdated();
    }
    else
    {
        processSchedules();

        // If the resumed schedule's date has already passed,
        // immediately turn the newly generated event into Overdue.
        checkEventStatuses();
    }

    emit schedulesUpdated();

    return true;
}

void MaintenanceManager::removePendingEventForSchedule(
    const QUuid& scheduleId,
    const QDate& scheduledDate
    )
{
    for (int i = m_events.size() - 1; i >= 0; --i)
    {
        const MaintenanceEventData& data =
            m_events[i].data();

        if (data.scheduleId != scheduleId)
            continue;

        if (data.scheduledDate != scheduledDate)
            continue;

        if (data.status != MaintenanceStatus::Scheduled &&
            data.status != MaintenanceStatus::Overdue)
        {
            continue;
        }

        m_events.removeAt(i);
    }
}

QDate MaintenanceManager::calculateNextScheduledDate(
    const MaintenanceSchedule& schedule,
    const QDate& fromDate
    ) const
{
    switch (schedule.intervalType)
    {
    case MaintenanceIntervalType::Days:
        return fromDate.addDays(schedule.interval);

    case MaintenanceIntervalType::Weeks:
        return fromDate.addDays(schedule.interval * 7);

    case MaintenanceIntervalType::Months:
        return fromDate.addMonths(schedule.interval);

    case MaintenanceIntervalType::Years:
        return fromDate.addYears(schedule.interval);
    }

    return fromDate;
}

bool MaintenanceManager::completeEvent(
    const QUuid& eventId,
    const QDate& completionDate
    )
{
    MaintenanceEvent* event =
        getEvent(eventId);

    if (!event)
        return false;

    if (!completionDate.isValid())
        return false;

    MaintenanceEventData& eventData =
        event->data();

    if (eventData.status == MaintenanceStatus::Completed)
        return false;

    eventData.status =
        MaintenanceStatus::Completed;

    eventData.completedTime =
        QDateTime(
            completionDate,
            QTime(0, 0)
            );

    bool schedulesChanged = false;

    for (const QUuid& scheduleId :
         eventData.affectedScheduleIds)
    {
        MaintenanceSchedule* schedule =
            getSchedule(scheduleId);

        if (!schedule)
            continue;

        if (schedule->status !=
            MaintenanceScheduleStatus::Active)
        {
            continue;
        }

        schedule->nextScheduledDate =
            calculateNextScheduledDate(
                *schedule,
                completionDate
                );

        schedulesChanged = true;
    }

    emit eventsUpdated();

    if (schedulesChanged)
        emit schedulesUpdated();

    return true;
}

QList<MaintenanceEventData>
MaintenanceManager::eventsForDate(
    const QDate& date
    ) const
{
    QList<MaintenanceEventData> result;

    if (!date.isValid())
        return result;

    // ---------------------------------------------------------
    // 1. Real/materialized events
    // ---------------------------------------------------------

    for (const MaintenanceEvent& event : m_events)
    {
        const MaintenanceEventData& data =
            event.data();

        if (data.scheduledDate == date)
            result.append(data);
    }

    // ---------------------------------------------------------
    // 2. Future schedule occurrences
    // ---------------------------------------------------------

    for (const MaintenanceSchedule& schedule : m_schedules)
    {
        if (schedule.status !=
            MaintenanceScheduleStatus::Active)
        {
            continue;
        }

        if (!schedule.nextScheduledDate.isValid() ||
            schedule.interval <= 0)
        {
            continue;
        }

        // The schedule has already moved beyond this date.
        if (date < schedule.nextScheduledDate)
            continue;

        QDate occurrence =
            schedule.nextScheduledDate;

        while (occurrence < date)
        {
            occurrence =
                calculateNextScheduledDate(
                    schedule,
                    occurrence
                    );

            if (!occurrence.isValid())
                break;
        }

        if (occurrence != date)
            continue;

        // -----------------------------------------------------
        // Check whether this occurrence already exists as
        // a real event.
        // -----------------------------------------------------

        bool alreadyExists = false;

        for (const MaintenanceEvent& event : m_events)
        {
            const MaintenanceEventData& data =
                event.data();

            if (data.scheduleId == schedule.id &&
                data.scheduledDate == date)
            {
                alreadyExists = true;
                break;
            }
        }

        if (alreadyExists)
            continue;

        // -----------------------------------------------------
        // Build a temporary projected event.
        // -----------------------------------------------------

        MaintenanceEventData data;

        data.id = QUuid::createUuid();

        data.title =
            schedule.title;

        data.assetId =
            schedule.assetId;

        data.subAssetId =
            schedule.subAssetId;

        if (Asset* asset =
            m_assetManager.getAsset(
                schedule.assetId))
        {
            data.assetName =
                asset->info().name;

            if (!schedule.subAssetId.isNull())
            {
                for (const SubAsset& subAsset :
                     asset->subAssets())
                {
                    if (subAsset.id() ==
                        schedule.subAssetId)
                    {
                        data.subAssetName =
                            subAsset.info().name;

                        break;
                    }
                }
            }
        }

        data.scheduledDate =
            date;

        data.status =
            MaintenanceStatus::Scheduled;

        data.type =
            MaintenanceType::PM;

        data.scheduleId =
            schedule.id;

        result.append(data);
    }

    return result;
}

QDate MaintenanceManager::nextScheduledEventDate() const
{
    const QDate today =
        QDate::currentDate();

    QDate result;

    // Real future events
    for (const MaintenanceEvent& event : m_events)
    {
        const QDate date =
            event.data().scheduledDate;

        if (!date.isValid() ||
            date <= today)
        {
            continue;
        }

        if (!result.isValid() ||
            date < result)
        {
            result = date;
        }
    }

    // Future schedule occurrences
    for (const MaintenanceSchedule& schedule :
         m_schedules)
    {
        if (schedule.status !=
            MaintenanceScheduleStatus::Active)
        {
            continue;
        }

        if (!schedule.nextScheduledDate.isValid() ||
            schedule.interval <= 0)
        {
            continue;
        }

        QDate nextDate =
            schedule.nextScheduledDate;

        while (nextDate <= today)
        {
            const QDate advanced =
                calculateNextScheduledDate(
                    schedule,
                    nextDate
                    );

            if (advanced == nextDate)
                break;

            nextDate = advanced;
        }

        if (!nextDate.isValid() ||
            nextDate <= today)
        {
            continue;
        }

        if (!result.isValid() ||
            nextDate < result)
        {
            result = nextDate;
        }
    }

    return result;
}

bool MaintenanceManager::removeAssetData(
    const QUuid& assetId
    )
{
    bool eventsChanged = false;
    bool schedulesChanged = false;

    // Remove all events belonging to this asset.
    for (int i = m_events.size() - 1; i >= 0; --i)
    {
        const MaintenanceEventData& data =
            m_events[i].data();

        if (data.assetId != assetId)
            continue;

        m_events.removeAt(i);
        eventsChanged = true;
    }

    // Remove all schedules belonging to this asset.
    for (int i = m_schedules.size() - 1; i >= 0; --i)
    {
        if (m_schedules[i].assetId != assetId)
            continue;

        m_schedules.removeAt(i);
        schedulesChanged = true;
    }

    if (eventsChanged)
        emit eventsUpdated();

    if (schedulesChanged)
        emit schedulesUpdated();

    return eventsChanged || schedulesChanged;
}

void MaintenanceManager::clearData()
{
    m_events.clear();
    m_schedules.clear();
}

void MaintenanceManager::restoreEvent(
    const MaintenanceEvent& event
    )
{
    m_events.append(event);
}

void MaintenanceManager::restoreSchedule(
    const MaintenanceSchedule& schedule
    )
{
    m_schedules.append(schedule);
}

bool MaintenanceManager::removeSubAssetData(
    const QUuid& assetId,
    const QUuid& subAssetId
    )
{
    bool eventsChanged = false;
    bool schedulesChanged = false;

    // Remove all events belonging to this subasset.
    for (int i = m_events.size() - 1; i >= 0; --i)
    {
        const MaintenanceEventData& data =
            m_events[i].data();

        if (data.assetId != assetId ||
            data.subAssetId != subAssetId)
        {
            continue;
        }

        m_events.removeAt(i);
        eventsChanged = true;
    }

    // Remove all schedules belonging to this subasset.
    for (int i = m_schedules.size() - 1; i >= 0; --i)
    {
        const MaintenanceSchedule& schedule =
            m_schedules[i];

        if (schedule.assetId != assetId ||
            schedule.subAssetId != subAssetId)
        {
            continue;
        }

        m_schedules.removeAt(i);
        schedulesChanged = true;
    }

    if (eventsChanged)
        emit eventsUpdated();

    if (schedulesChanged)
        emit schedulesUpdated();

    return eventsChanged || schedulesChanged;
}

QList<MaintenanceEventData>
MaintenanceManager::eventsForFilter(
    const MaintenanceEventFilter& filter
    ) const
{
    QList<MaintenanceEventData> result;

    for (const MaintenanceEvent& event : m_events)
    {
        const MaintenanceEventData& data =
            event.data();

        if (!data.scheduledDate.isValid())
            continue;

        if (filter.fromDate.isValid() &&
            data.scheduledDate < filter.fromDate)
        {
            continue;
        }

        if (filter.toDate.isValid() &&
            data.scheduledDate > filter.toDate)
        {
            continue;
        }

        if (filter.assetFilterEnabled &&
            !filter.assetIds.contains(data.assetId))
        {
            continue;
        }

        if (!filter.subAssetIds.isEmpty() &&
            !filter.subAssetIds.contains(data.subAssetId))
        {
            continue;
        }

        if (!filter.types.isEmpty() &&
            !filter.types.contains(data.type))
        {
            continue;
        }

        if (!filter.statuses.isEmpty() &&
            !filter.statuses.contains(data.status))
        {
            continue;
        }

        result.append(data);
    }

    return result;
}