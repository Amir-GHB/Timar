#pragma once

#include <QObject>
#include <QDate>
#include <QList>
#include <QString>
#include <QUuid>
#include "subasset.h"

struct AssetPart
{
	QUuid id;
	QUuid storagePartId;
	QDate installationDate;
};

struct AssetInfo
{
    QString name;
    QString serialNumber;
    QString manufacturer;
    QString model;

    QDate installationDate;
    QString installationPlace;

    QString assetType;
    QString imagePath;
    QString description;
    QList<AssetPart> parts;
};

class Asset
{
public:
    Asset();

    explicit Asset(
        const AssetInfo& info
    );

    Asset(
        const QUuid& id,
        const AssetInfo& info
    );
    
    QUuid id() const;

    const AssetInfo& info() const;
    AssetInfo& info();
    void setInfo(
        const AssetInfo& info
    );

    void addSubAsset(
        const SubAsset& subAsset
    );

    const QList<SubAsset>& subAssets() const;
    QList<SubAsset>& subAssets();

    bool removeSubAsset(const QUuid& subAssetId);

private:
    QUuid m_id;
    AssetInfo m_info;
    QList<SubAsset> m_subAssets;
};


