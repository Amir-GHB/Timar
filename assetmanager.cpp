#include "assetmanager.h"

Asset* AssetManager::addAsset(const AssetInfo& info)
{
    m_assets.append(Asset(info));

    return &m_assets.last();
}

void AssetManager::removeAsset(const QUuid& id)
{
    for (auto it = m_assets.begin(); it != m_assets.end(); ++it)
    {
        if (it->id() == id)
        {
            m_assets.erase(it);
            return;
        }
    }
}

Asset* AssetManager::getAsset(const QUuid& id)
{
    for (auto& asset : m_assets)
    {
        if (asset.id() == id)
            return &asset;
    }

    return nullptr;
}

const QList<Asset>& AssetManager::assets() const
{
    return m_assets;
}

bool AssetManager::updateAsset(
    const QUuid& id,
    const AssetInfo& info
    )
{
    Asset* asset = getAsset(id);

    if (!asset)
        return false;

    asset->setInfo(info);

    return true;
}

bool AssetManager::addSubAsset(
    const QUuid& assetId,
    const SubAssetInfo& info
    )
{
    Asset* asset =
        getAsset(assetId);

    if (!asset)
        return false;

    asset->addSubAsset(
        SubAsset(info)
        );

    return true;
}

void AssetManager::clear()
{
    m_assets.clear();
}

void AssetManager::restoreAsset(
    const Asset& asset
    )
{
    m_assets.append(asset);
}

bool AssetManager::updateSubAsset(
    const QUuid& assetId,
    const QUuid& subAssetId,
    const SubAssetInfo& info
    )
{
    Asset* asset =
        getAsset(assetId);

    if (!asset)
        return false;

    for (SubAsset& subAsset :
         asset->subAssets())
    {
        if (subAsset.id() != subAssetId)
            continue;

        subAsset.setInfo(info);
        return true;
    }

    return false;
}

bool AssetManager::removeSubAsset(
    const QUuid& assetId,
    const QUuid& subAssetId
    )
{
    Asset* asset =
        getAsset(assetId);

    if (!asset)
        return false;

    return asset->removeSubAsset(subAssetId);
}