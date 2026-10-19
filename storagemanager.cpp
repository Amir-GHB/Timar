#include "storagemanager.h"

StorageManager::StorageManager(
    AssetManager& assetManager,
    QObject* parent
    )
    : QObject(parent)
    , m_assetManager(assetManager)
{
}

const QList<StoragePart>& StorageManager::parts() const
{
    return m_parts;
}

void StorageManager::addPart(const StoragePart& part)
{
    m_parts.append(part);
    emit partsUpdated();
}

void StorageManager::removePart(
    const QUuid& id
    )
{
    for (int i = 0; i < m_parts.size(); ++i)
    {
        if (m_parts[i].id != id)
            continue;

        m_parts.removeAt(i);
        emit partsUpdated();
        return;
    }
}
void StorageManager::increaseQuantity(
    const QUuid& id
    )
{
    for (auto& part : m_parts)
    {
        if (part.id == id)
        {
            ++part.quantity;
            emit partsUpdated();
            return;
        }
    }
}

void StorageManager::decreaseQuantity(
    const QUuid& id
    )
{
    for (auto& part : m_parts)
    {
        if (part.id == id && part.quantity > 0)
        {
            --part.quantity;
            emit partsUpdated();
            return;
        }
    }
}
void StorageManager::updatePart(
    const StoragePart& updatedPart
    )
{
    for (auto& part : m_parts)
    {
        if (part.id == updatedPart.id)
        {
            part = updatedPart;
            emit partsUpdated();
            return;
        }
    }
}
void StorageManager::clear()
{
    m_parts.clear();
}

void StorageManager::restorePart(
    const StoragePart& part
    )
{
    m_parts.append(part);
}

bool StorageManager::installPart(
    const QUuid& assetId,
    const QUuid& storagePartId
    )
{
    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (!asset)
        return false;

    for (auto& part : m_parts)
    {
        if (part.id != storagePartId)
            continue;

        if (part.quantity <= 0)
            return false;

        AssetPart assetPart;

        assetPart.id =
            QUuid::createUuid();

        assetPart.storagePartId =
            storagePartId;

        assetPart.installationDate =
            QDate::currentDate();

        asset->info().parts.append(
            assetPart
            );

        --part.quantity;

        emit partsUpdated();

        return true;
    }

    return false;
}

bool StorageManager::removePartFromAsset(
    const QUuid& assetId,
    const QUuid& assetPartId
    )
{
    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (!asset)
        return false;

    auto& parts =
        asset->info().parts;

    for (int i = 0; i < parts.size(); ++i)
    {
        if (parts[i].id != assetPartId)
            continue;

        const QUuid storagePartId =
            parts[i].storagePartId;

        parts.removeAt(i);

        emit partsUpdated();

        return true;
    }

    return false;
}

bool StorageManager::replacePart(
    const QUuid& assetId,
    const QUuid& assetPartId
    )
{
    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (!asset)
        return false;

    for (auto& assetPart :
         asset->info().parts)
    {
        if (assetPart.id != assetPartId)
            continue;

        StoragePart* storagePart =
            nullptr;

        for (auto& part : m_parts)
        {
            if (part.id == assetPart.storagePartId)
            {
                storagePart = &part;
                break;
            }
        }

        if (!storagePart ||
            storagePart->quantity <= 0)
        {
            return false;
        }

        --storagePart->quantity;

        assetPart.installationDate =
            QDate::currentDate();

        emit partsUpdated();

        return true;
    }

    return false;
}

const StoragePart* StorageManager::getPart(
    const QUuid& id
    ) const
{
    for (const auto& part : m_parts)
    {
        if (part.id == id)
            return &part;
    }

    return nullptr;
}