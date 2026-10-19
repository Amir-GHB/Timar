#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "asset.h"

#include <QList>

class AssetManager
{
public:
    AssetManager() = default;

    Asset* addAsset(
        const AssetInfo& info
        );

    void removeAsset(
        const QUuid& id
        );

    Asset* getAsset(
        const QUuid& id
        );

    const QList<Asset>& assets() const;

    bool updateAsset(
        const QUuid& id,
        const AssetInfo& info
        );

    bool addSubAsset(
        const QUuid& assetId,
        const SubAssetInfo& info
        );

    void clear();

    void restoreAsset(
        const Asset& asset
        );

    bool updateSubAsset(
        const QUuid& assetId,
        const QUuid& subAssetId,
        const SubAssetInfo& info
        );

    bool removeSubAsset(
        const QUuid& assetId,
        const QUuid& subAssetId
        );

    bool removeSubAssetData(
        const QUuid& assetId,
        const QUuid& subAssetId
        );

private:
    QList<Asset> m_assets;
};

#endif