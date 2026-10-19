#include "datamanager.h"
//#include "storagemanager.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

/*static QJsonArray maintenancePartsToJson(
    const QList<MaintenancePart>& parts)
{
    QJsonArray array;

    for (const MaintenancePart& part : parts)
    {
        QJsonObject object;

        object["storagePartId"] =
            part.storagePartId.toString(QUuid::WithoutBraces);

        object["partName"] = part.partName;
        object["partNumber"] = part.partNumber;
        object["plannedQuantity"] = part.plannedQuantity;
        object["usedQuantity"] = part.usedQuantity;

        array.append(object);
    }

    return array;
}*/

/*static bool jsonToMaintenanceParts(
    const QJsonValue& value,
    QList<MaintenancePart>& parts)
{
    // Old saved data has no parts field.
    if (value.isUndefined())
        return true;

    if (!value.isArray())
        return false;

    QList<MaintenancePart> parsedParts;

    for (const QJsonValue& entry : value.toArray())
    {
        if (!entry.isObject())
            return false;

        const QJsonObject object = entry.toObject();

        MaintenancePart part;

        part.storagePartId =
            QUuid(object.value("storagePartId").toString());

        part.partName =
            object.value("partName").toString();

        part.partNumber =
            object.value("partNumber").toString();

        part.plannedQuantity =
            object.value("plannedQuantity").toInt();

        part.usedQuantity =
            object.value("usedQuantity").toInt();

        if (part.storagePartId.isNull() ||
            part.plannedQuantity < 0 ||
            part.usedQuantity < 0)
        {
            return false;
        }

        parsedParts.append(part);
    }

    parts = parsedParts;
    return true;
}
*/
DataManager::DataManager(
    AssetManager& assetManager,
    MaintenanceManager& maintenanceManager,
    StorageManager& storageManager,
    QObject* parent
    )
    : QObject(parent)
    , m_assetManager(assetManager)
    , m_maintenanceManager(maintenanceManager)
    , m_storageManager(storageManager)
{
    const QString appDataPath =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QDir dir;

    dir.mkpath(appDataPath);

    m_defaultFilePath =
        appDataPath + "/data.json";
}

QString DataManager::defaultFilePath() const
{
    return m_defaultFilePath;
}

bool DataManager::save()
{
    return exportData(
        m_defaultFilePath
        );
}

bool DataManager::load()
{
    QFile file(m_defaultFilePath);

    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly))
        return false;

    const QByteArray data =
        file.readAll();

    file.close();

    QJsonParseError error;

    const QJsonDocument document =
        QJsonDocument::fromJson(
            data,
            &error
            );

    if (error.error != QJsonParseError::NoError)
        return false;

    if (!document.isObject())
        return false;

    return loadJson(
        document.object()
        );
}

bool DataManager::exportData(
    const QString& filePath
    )
{
    if (filePath.isEmpty())
        return false;

    const QJsonObject root =
        createJson();

    const QJsonDocument document(
        root
        );

    QFile file(filePath);

    if (!file.open(
            QIODevice::WriteOnly |
            QIODevice::Truncate
            ))
    {
        return false;
    }

    file.write(
        document.toJson(
            QJsonDocument::Indented
            )
        );

    file.close();

    return true;
}

bool DataManager::importData(
    const QString& filePath
    )
{
    if (filePath.isEmpty())
        return false;

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    const QByteArray data =
        file.readAll();

    file.close();

    QJsonParseError error;

    const QJsonDocument document =
        QJsonDocument::fromJson(
            data,
            &error
            );

    if (error.error != QJsonParseError::NoError)
        return false;

    if (!document.isObject())
        return false;

    return loadJson(
        document.object()
        );
}

QJsonObject DataManager::createJson() const
{
    QJsonObject root;

    root["version"] = 1;

    /*
     * Assets
     */

    QJsonArray assetsArray;

    for (const Asset& asset :
         m_assetManager.assets())
    {
        assetsArray.append(
            assetToJson(asset)
            );
    }

    root["assets"] =
        assetsArray;

    /*
     * Maintenance events
     */

    QJsonArray eventsArray;

    for (const MaintenanceEvent& event :
         m_maintenanceManager.events())
    {
        eventsArray.append(
            eventToJson(event)
            );
    }

    root["events"] =
        eventsArray;

    /*
     * Maintenance schedules
     */

    QJsonArray schedulesArray;

    for (const MaintenanceSchedule& schedule :
         m_maintenanceManager.schedules())
    {
        schedulesArray.append(
            scheduleToJson(schedule)
            );
    }

    root["schedules"] =
        schedulesArray;

    //parts

   QJsonArray storagePartsArray;

    for (const StoragePart& part :
         m_storageManager.parts())
    {
        QJsonObject object;

        object["id"] =
            part.id.toString(
                QUuid::WithoutBraces
                );

        object["name"] =
            part.name;

        object["partNumber"] =
            part.partNumber;

        object["description"] =
            part.description;

        object["quantity"] =
            part.quantity;

        storagePartsArray.append(object);
    }

    root["storageParts"] =
        storagePartsArray;

    return root;
}

bool DataManager::loadJson(
    const QJsonObject& root
    )
{
    /*
     * Basic version check.
     */

    const int version =
        root.value("version").toInt(-1);

    if (version != 1)
        return false;

    if (!root.value("assets").isArray())
        return false;

    if (!root.value("events").isArray())
        return false;

    if (!root.value("schedules").isArray())
        return false;

    /*
     * Parse everything into temporary containers first.
     *
     * This prevents a malformed file from partially
     * destroying the currently loaded data.
     */

    QList<Asset> importedAssets;

    const QJsonArray assetsArray =
        root.value("assets").toArray();

    for (const QJsonValue& value :
         assetsArray)
    {
        if (!value.isObject())
            return false;

        Asset asset;

        if (!jsonToAsset(
                value.toObject(),
                asset
                ))
        {
            return false;
        }

        importedAssets.append(asset);
    }

    QVector<MaintenanceEvent> importedEvents;

    const QJsonArray eventsArray =
        root.value("events").toArray();

    for (const QJsonValue& value :
         eventsArray)
    {
        if (!value.isObject())
            return false;

        MaintenanceEvent event;

        if (!jsonToEvent(
                value.toObject(),
                event
                ))
        {
            return false;
        }

        importedEvents.append(event);
    }

    QVector<MaintenanceSchedule> importedSchedules;

    const QJsonArray schedulesArray =
        root.value("schedules").toArray();

    for (const QJsonValue& value :
         schedulesArray)
    {
        if (!value.isObject())
            return false;

        MaintenanceSchedule schedule;

        if (!jsonToSchedule(
                value.toObject(),
                schedule
                ))
        {
            return false;
        }

        importedSchedules.append(schedule);
    }

   QList<StoragePart> importedStorageParts;

    if (root.value("storageParts").isArray())
    {
        const QJsonArray storagePartsArray =
            root.value("storageParts").toArray();

        for (const QJsonValue& value :
             storagePartsArray)
        {
            if (!value.isObject())
                return false;

            const QJsonObject object =
                value.toObject();

            StoragePart part;

            part.id =
                QUuid(
                    object.value("id").toString()
                    );

            if (part.id.isNull())
                return false;

            part.name =
                object.value("name").toString();

            part.partNumber =
                object.value("partNumber").toString();

            part.description =
                object.value("description").toString();

            part.quantity =
                object.value("quantity").toInt();

            importedStorageParts.append(part);
        }
    }

    /*
     * Everything parsed successfully.
     *
     * Replace the current data.
     */

    m_assetManager.clear();

    for (const Asset& asset :
         importedAssets)
    {
        m_assetManager.restoreAsset(asset);
    }

    m_maintenanceManager.clearData();

    for (const MaintenanceEvent& event :
         importedEvents)
    {
        m_maintenanceManager.restoreEvent(
            event
            );
    }

    for (const MaintenanceSchedule& schedule :
         importedSchedules)
    {
        m_maintenanceManager.restoreSchedule(
            schedule
            );
    }

  m_storageManager.clear();

    for (const StoragePart& part :
         importedStorageParts)
    {
        m_storageManager.restorePart(part);
    }

    emit m_maintenanceManager.eventsUpdated();
    emit m_maintenanceManager.schedulesUpdated();

    return true;
}

QJsonObject DataManager::assetToJson(
    const Asset& asset
    ) const
{
    QJsonObject object;

    const AssetInfo& info =
        asset.info();

    object["id"] =
        asset.id().toString(
            QUuid::WithoutBraces
            );

    object["name"] =
        info.name;

    object["serialNumber"] =
        info.serialNumber;

    object["manufacturer"] =
        info.manufacturer;

    object["model"] =
        info.model;

    object["installationDate"] =
        info.installationDate.toString(
            Qt::ISODate
            );

    object["installationPlace"] =
        info.installationPlace;

    object["assetType"] =
        info.assetType;

    object["imagePath"] =
        info.imagePath;

    object["description"] =
        info.description;

    QJsonArray partsArray;

    for (const AssetPart& part : info.parts)
    {
        QJsonObject partObject;

        partObject["id"] =
            part.id.toString(
                QUuid::WithoutBraces
                );

        partObject["storagePartId"] =
            part.storagePartId.toString(
                QUuid::WithoutBraces
                );

        partObject["installationDate"] =
            part.installationDate.toString(
                Qt::ISODate
                );

        partsArray.append(partObject);
    }

    object["parts"] = partsArray;

    QJsonArray subAssetsArray;

    for (const SubAsset& subAsset :
         asset.subAssets())
    {
        subAssetsArray.append(
            subAssetToJson(subAsset)
            );
    }

    object["subAssets"] =
        subAssetsArray;

    return object;
}

QJsonObject DataManager::subAssetToJson(
    const SubAsset& subAsset
    ) const
{
    QJsonObject object;

    const SubAssetInfo& info =
        subAsset.info();

    object["id"] =
        subAsset.id().toString(
            QUuid::WithoutBraces
            );

    object["name"] =
        info.name;

    object["serialNumber"] =
        info.serialNumber;

    object["manufacturer"] =
        info.manufacturer;

    object["model"] =
        info.model;

    object["type"] =
        info.type;

    object["installationDate"] =
        info.installationDate.toString(
            Qt::ISODate
            );

    object["installationPlace"] =
        info.installationPlace;

    object["imagePath"] =
        info.imagePath;

    object["description"] =
        info.description;

    object["status"] =
        info.status;

    return object;
}

QJsonObject DataManager::eventToJson(
    const MaintenanceEvent& event
    ) const
{
    const MaintenanceEventData& data =
        event.data();

    QJsonObject object;

    object["id"] =
        data.id.toString(
            QUuid::WithoutBraces
            );

    object["title"] =
        data.title;

    object["description"] =
        data.description;

    object["assetName"] =
        data.assetName;

    object["subAssetName"] =
        data.subAssetName;

    object["facilitySection"] =
        data.facilitySection;

    object["type"] =
        maintenanceTypeToString(
            data.type
            );

    object["scheduledDate"] =
        data.scheduledDate.toString(
            Qt::ISODate
            );

    object["completedTime"] =
        data.completedTime.isValid()
            ? data.completedTime.toString(
                  Qt::ISODateWithMs
                  )
            : QString();

    object["status"] =
        maintenanceStatusToString(
            data.status
            );

    object["assetId"] =
        data.assetId.toString(
            QUuid::WithoutBraces
            );

    object["subAssetId"] =
        data.subAssetId.toString(
            QUuid::WithoutBraces
            );

    object["scheduleId"] =
        data.scheduleId.toString(
            QUuid::WithoutBraces
            );

    object["technician"] =
        data.technician;

    QJsonArray affectedSchedules;

    for (const QUuid& id :
         data.affectedScheduleIds)
    {
        affectedSchedules.append(
            id.toString(
                QUuid::WithoutBraces
                )
            );
    }

    object["affectedScheduleIds"] =
        affectedSchedules;

    /*object["parts"] = maintenancePartsToJson(data.parts);*/

    return object;
}

QJsonObject DataManager::scheduleToJson(
    const MaintenanceSchedule& schedule
    ) const
{
    QJsonObject object;

    object["id"] =
        schedule.id.toString(
            QUuid::WithoutBraces
            );

    object["assetId"] =
        schedule.assetId.toString(
            QUuid::WithoutBraces
            );

    object["subAssetId"] =
        schedule.subAssetId.toString(
            QUuid::WithoutBraces
            );

    object["title"] =
        schedule.title;

    object["interval"] =
        schedule.interval;

    object["intervalType"] =
        intervalTypeToString(
            schedule.intervalType
            );

    object["nextScheduledDate"] =
        schedule.nextScheduledDate.toString(
            Qt::ISODate
            );

    object["status"] =
        scheduleStatusToString(
            schedule.status
            );

   /* object["plannedParts"] =
        maintenancePartsToJson(schedule.plannedParts);*/

    return object;
}

bool DataManager::jsonToAsset(
    const QJsonObject& object,
    Asset& asset
    ) const
{
    const QUuid id(
        object.value("id").toString()
        );

    if (id.isNull())
        return false;

    AssetInfo info;

    info.name =
        object.value("name").toString();

    info.serialNumber =
        object.value("serialNumber").toString();

    info.manufacturer =
        object.value("manufacturer").toString();

    info.model =
        object.value("model").toString();

    info.installationDate =
        QDate::fromString(
            object.value(
                      "installationDate"
                      ).toString(),
            Qt::ISODate
            );

    info.installationPlace =
        object.value(
                  "installationPlace"
                  ).toString();

    info.assetType =
        object.value(
                  "assetType"
                  ).toString();

    info.imagePath =
        object.value(
                  "imagePath"
                  ).toString();

    info.description =
        object.value(
                  "description"
                  ).toString();

    const QJsonArray parts =
        object.value("parts").toArray();

    for (const QJsonValue& value : parts)
    {
        if (!value.isObject())
            return false;

        const QJsonObject partObject =
            value.toObject();

        AssetPart part;

        part.id =
            QUuid(
                partObject.value("id").toString()
                );

        part.storagePartId =
            QUuid(
                partObject.value("storagePartId").toString()
                );

        part.installationDate =
            QDate::fromString(
                partObject.value(
                              "installationDate"
                              ).toString(),
                Qt::ISODate
                );

        if (part.id.isNull() ||
            part.storagePartId.isNull())
        {
            return false;
        }

        info.parts.append(part);
    }

    asset =
        Asset(
            id,
            info
            );

    const QJsonArray subAssets =
        object.value(
                  "subAssets"
                  ).toArray();

    for (const QJsonValue& value :
         subAssets)
    {
        if (!value.isObject())
            return false;

        SubAsset subAsset;

        if (!jsonToSubAsset(
                value.toObject(),
                subAsset
                ))
        {
            return false;
        }

        asset.addSubAsset(
            subAsset
            );
    }

    return true;
}

bool DataManager::jsonToSubAsset(
    const QJsonObject& object,
    SubAsset& subAsset
    ) const
{
    const QUuid id(
        object.value("id").toString()
        );

    if (id.isNull())
        return false;

    SubAssetInfo info;

    info.name =
        object.value("name").toString();

    info.serialNumber =
        object.value("serialNumber").toString();

    info.manufacturer =
        object.value("manufacturer").toString();

    info.model =
        object.value("model").toString();

    info.type =
        object.value("type").toString();

    info.installationDate =
        QDate::fromString(
            object.value(
                      "installationDate"
                      ).toString(),
            Qt::ISODate
            );

    info.installationPlace =
        object.value(
                  "installationPlace"
                  ).toString();

    info.imagePath =
        object.value(
                  "imagePath"
                  ).toString();

    info.description =
        object.value(
                  "description"
                  ).toString();

    info.status =
        object.value(
                  "status"
                  ).toString();

    subAsset =
        SubAsset(
            id,
            info
            );

    return true;
}

bool DataManager::jsonToEvent(
    const QJsonObject& object,
    MaintenanceEvent& event
    ) const
{
    MaintenanceEventData data;

    data.id =
        QUuid(
            object.value("id").toString()
            );

    if (data.id.isNull())
        return false;

    data.title =
        object.value("title").toString();

    data.description =
        object.value("description").toString();

    data.assetName =
        object.value("assetName").toString();

    data.subAssetName =
        object.value("subAssetName").toString();

    data.facilitySection =
        object.value(
                  "facilitySection"
                  ).toString();

    data.type =
        stringToMaintenanceType(
            object.value("type").toString()
            );

    data.scheduledDate =
        QDate::fromString(
            object.value(
                      "scheduledDate"
                      ).toString(),
            Qt::ISODate
            );

    const QString completedTime =
        object.value(
                  "completedTime"
                  ).toString();

    if (!completedTime.isEmpty())
    {
        data.completedTime =
            QDateTime::fromString(
                completedTime,
                Qt::ISODateWithMs
                );
    }

    data.status =
        stringToMaintenanceStatus(
            object.value("status").toString()
            );

    data.assetId =
        QUuid(
            object.value(
                      "assetId"
                      ).toString()
            );

    data.subAssetId =
        QUuid(
            object.value(
                      "subAssetId"
                      ).toString()
            );

    data.scheduleId =
        QUuid(
            object.value(
                      "scheduleId"
                      ).toString()
            );

    data.technician =
        object.value(
                  "technician"
                  ).toString();

    const QJsonArray affected =
        object.value(
                  "affectedScheduleIds"
                  ).toArray();

    for (const QJsonValue& value :
         affected)
    {
        const QUuid id(
            value.toString()
            );

        if (!id.isNull())
            data.affectedScheduleIds.append(id);
    }

 /*   if (!jsonToMaintenanceParts(
            object.value("parts"),
            data.parts))
    {
        return false;
    }
*/
    event =
        MaintenanceEvent(data);

    return true;
}

bool DataManager::jsonToSchedule(
    const QJsonObject& object,
    MaintenanceSchedule& schedule
    ) const
{
    schedule.id =
        QUuid(
            object.value("id").toString()
            );

    if (schedule.id.isNull())
        return false;

    schedule.assetId =
        QUuid(
            object.value(
                      "assetId"
                      ).toString()
            );

    schedule.subAssetId =
        QUuid(
            object.value(
                      "subAssetId"
                      ).toString()
            );

    schedule.title =
        object.value(
                  "title"
                  ).toString();

    schedule.interval =
        object.value(
                  "interval"
                  ).toInt();

    schedule.intervalType =
        stringToIntervalType(
            object.value(
                      "intervalType"
                      ).toString()
            );

    schedule.nextScheduledDate =
        QDate::fromString(
            object.value(
                      "nextScheduledDate"
                      ).toString(),
            Qt::ISODate
            );

    schedule.status =
        stringToScheduleStatus(
            object.value(
                      "status"
                      ).toString()
            );

  /*  if (!jsonToMaintenanceParts(
            object.value("plannedParts"),
            schedule.plannedParts))
    {
        return false;
    }
*/
    return true;
}

QString DataManager::maintenanceTypeToString(
    MaintenanceType type
    ) const
{
    switch (type)
    {
    case MaintenanceType::PM:
        return "PM";

    case MaintenanceType::EM:
        return "EM";
    }

    return "PM";
}

MaintenanceType DataManager::stringToMaintenanceType(
    const QString& value
    ) const
{
    if (value == "EM")
        return MaintenanceType::EM;

    return MaintenanceType::PM;
}

QString DataManager::maintenanceStatusToString(
    MaintenanceStatus status
    ) const
{
    switch (status)
    {
    case MaintenanceStatus::Scheduled:
        return "Scheduled";

    case MaintenanceStatus::InProgress:
        return "InProgress";

    case MaintenanceStatus::WaitingForHandling:
        return "WaitingForHandling";

    case MaintenanceStatus::Completed:
        return "Completed";

    case MaintenanceStatus::Cancelled:
        return "Cancelled";

    case MaintenanceStatus::Overdue:
        return "Overdue";

    case MaintenanceStatus::Missed:
        return "Missed";
    }

    return "Scheduled";
}

MaintenanceStatus DataManager::stringToMaintenanceStatus(
    const QString& value
    ) const
{
    if (value == "InProgress")
        return MaintenanceStatus::InProgress;

    if (value == "WaitingForHandling")
        return MaintenanceStatus::WaitingForHandling;

    if (value == "Completed")
        return MaintenanceStatus::Completed;

    if (value == "Cancelled")
        return MaintenanceStatus::Cancelled;

    if (value == "Overdue")
        return MaintenanceStatus::Overdue;

    if (value == "Missed")
        return MaintenanceStatus::Missed;

    return MaintenanceStatus::Scheduled;
}

QString DataManager::intervalTypeToString(
    MaintenanceIntervalType type
    ) const
{
    switch (type)
    {
    case MaintenanceIntervalType::Days:
        return "Days";

    case MaintenanceIntervalType::Weeks:
        return "Weeks";

    case MaintenanceIntervalType::Months:
        return "Months";

    case MaintenanceIntervalType::Years:
        return "Years";
    }

    return "Days";
}

MaintenanceIntervalType DataManager::stringToIntervalType(
    const QString& value
    ) const
{
    if (value == "Weeks")
        return MaintenanceIntervalType::Weeks;

    if (value == "Months")
        return MaintenanceIntervalType::Months;

    if (value == "Years")
        return MaintenanceIntervalType::Years;

    return MaintenanceIntervalType::Days;
}

QString DataManager::scheduleStatusToString(
    MaintenanceScheduleStatus status
    ) const
{
    switch (status)
    {
    case MaintenanceScheduleStatus::Active:
        return "Active";

    case MaintenanceScheduleStatus::Paused:
        return "Paused";
    }

    return "Active";
}

MaintenanceScheduleStatus
DataManager::stringToScheduleStatus(
    const QString& value
    ) const
{
    if (value == "Paused")
        return MaintenanceScheduleStatus::Paused;

    return MaintenanceScheduleStatus::Active;
}