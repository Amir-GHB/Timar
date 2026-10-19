#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "assetmanager.h"
#include "maintenancemanager.h"
#include "storagemanager.h"
//#include "storagemanager.h"

#include <QObject>
#include <QString>

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(
        AssetManager& assetManager,
        MaintenanceManager& maintenanceManager,
        StorageManager& storageManager,
        QObject* parent = nullptr
        );

    bool save();

    bool load();

    bool exportData(
        const QString& filePath
        );

    bool importData(
        const QString& filePath
        );

    QString defaultFilePath() const;

private:
    QJsonObject createJson() const;

    bool loadJson(
        const QJsonObject& root
        );

    QJsonObject assetToJson(
        const Asset& asset
        ) const;

    QJsonObject subAssetToJson(
        const SubAsset& subAsset
        ) const;

    QJsonObject eventToJson(
        const MaintenanceEvent& event
        ) const;

    QJsonObject scheduleToJson(
        const MaintenanceSchedule& schedule
        ) const;

    bool jsonToAsset(
        const QJsonObject& object,
        Asset& asset
        ) const;

    bool jsonToSubAsset(
        const QJsonObject& object,
        SubAsset& subAsset
        ) const;

    bool jsonToEvent(
        const QJsonObject& object,
        MaintenanceEvent& event
        ) const;

    bool jsonToSchedule(
        const QJsonObject& object,
        MaintenanceSchedule& schedule
        ) const;

    QString maintenanceTypeToString(
        MaintenanceType type
        ) const;

    MaintenanceType stringToMaintenanceType(
        const QString& value
        ) const;

    QString maintenanceStatusToString(
        MaintenanceStatus status
        ) const;

    MaintenanceStatus stringToMaintenanceStatus(
        const QString& value
        ) const;

    QString intervalTypeToString(
        MaintenanceIntervalType type
        ) const;

    MaintenanceIntervalType stringToIntervalType(
        const QString& value
        ) const;

    QString scheduleStatusToString(
        MaintenanceScheduleStatus status
        ) const;

    MaintenanceScheduleStatus stringToScheduleStatus(
        const QString& value
        ) const;

private:
    AssetManager& m_assetManager;
    MaintenanceManager& m_maintenanceManager;
   StorageManager& m_storageManager;

    QString m_defaultFilePath;
};

#endif