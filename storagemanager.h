#pragma once

#include "storagepart.h"
#include "assetmanager.h"

#include <QObject>
#include <QList>

class StorageManager : public QObject
{
    Q_OBJECT

public:
    explicit StorageManager(
        AssetManager& assetManager,
        QObject* parent = nullptr
        );

    const QList<StoragePart>& parts() const;

    void addPart(const StoragePart& part);
    void removePart(const QUuid& id);

    bool installPart(
        const QUuid& assetId,
        const QUuid& storagePartId
        );

    bool removePartFromAsset(
        const QUuid& assetId,
        const QUuid& assetPartId
        );

    bool replacePart(
        const QUuid& assetId,
        const QUuid& assetPartId
        );

    void increaseQuantity(const QUuid& id);
    void decreaseQuantity(const QUuid& id);
    void updatePart(const StoragePart& part);

    void clear();

    void restorePart(const StoragePart& part);

    const StoragePart* getPart(
        const QUuid& id
        ) const;

signals:
    void partsUpdated();

private:
    AssetManager& m_assetManager;
    QList<StoragePart> m_parts;
};