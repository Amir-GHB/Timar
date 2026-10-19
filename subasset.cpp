#include "subasset.h"

SubAsset::SubAsset()
{
    m_id = QUuid::createUuid();
}

SubAsset::SubAsset(
    const SubAssetInfo& info
)
    : m_id(QUuid::createUuid()),
    m_info(info)
{
}

SubAsset::SubAsset(
    const QUuid& id,
    const SubAssetInfo& info
)
    : m_id(id),
    m_info(info)
{
    if (m_id.isNull())
        m_id = QUuid::createUuid();
}

QUuid SubAsset::id() const
{
    return m_id;
}

const SubAssetInfo& SubAsset::info() const
{
    return m_info;
}

void SubAsset::setInfo(
    const SubAssetInfo& info
)
{
    m_info = info;
}