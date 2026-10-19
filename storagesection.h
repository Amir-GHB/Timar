#pragma once

#include "storagemanager.h"

#include <QWidget>

class QVBoxLayout;

class StorageSection : public QWidget
{
    Q_OBJECT

public:
    explicit StorageSection(
        StorageManager& storageManager,
        QWidget* parent = nullptr
        );

private:
    void refreshParts();
    QWidget* createPartRow(const StoragePart& part);

    StorageManager& m_storageManager;

    QVBoxLayout* m_partsLayout = nullptr;

    bool showPartDialog(StoragePart& part, bool editing);
};