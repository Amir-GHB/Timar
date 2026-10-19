#include "asset.h"

Asset::Asset()
{
    m_id = QUuid::createUuid();
}

Asset::Asset(
    const AssetInfo& info
)
    : m_id(QUuid::createUuid()),
    m_info(info)
{
}

Asset::Asset(
    const QUuid& id,
    const AssetInfo& info
)
    : m_id(id),
    m_info(info)
{
    if (m_id.isNull())
        m_id = QUuid::createUuid();
}

QUuid Asset::id() const
{
    return m_id;
}

const AssetInfo& Asset::info() const
{
    return m_info;
}

AssetInfo& Asset::info()
{
    return m_info;
}

void Asset::setInfo(
    const AssetInfo& info
)
{
    m_info = info;
}

void Asset::addSubAsset(
    const SubAsset& subAsset
)
{
    m_subAssets.append(subAsset);
}

const QList<SubAsset>& Asset::subAssets() const
{
    return m_subAssets;
}

bool Asset::removeSubAsset(const QUuid& subAssetId)
{
    for (int i = 0; i < m_subAssets.size(); ++i)
    {
        if (m_subAssets[i].id() != subAssetId)
            continue;

        m_subAssets.removeAt(i);
        return true;
    }

    return false;
}

QList<SubAsset>& Asset::subAssets()
{
    return m_subAssets;
}