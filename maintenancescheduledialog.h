#ifndef MAINTENANCESCHEDULEDIALOG_H
#define MAINTENANCESCHEDULEDIALOG_H

#include "maintenanceschedule.h"
#include <QDialog>
#include <qdatetimeedit.h>
#include <QComboBox>
#include "storagemanager.h"
#include <QListWidget>

class SubAsset;
class QLineEdit;
class QDateTimeEdit;
class QComboBox;
class QSpinBox;
class QLabel;
class QPushButton;

class MaintenanceScheduleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaintenanceScheduleDialog(
        const QUuid& assetId,
        const QString& assetName,
        const QList<SubAsset>& subAssets,
        StorageManager& storageManager,
        QWidget* parent = nullptr
        );

    MaintenanceSchedule scheduleData() const;

private slots:
    void onCreateClicked();

private:
    void setupUI();

private:
    QUuid m_assetId;
    QString m_assetName;
    QList<SubAsset> m_subAssets;

    QLineEdit* m_titleEdit = nullptr;

    QComboBox* m_subAssetCombo = nullptr;

    QDateEdit* m_dateEdit = nullptr;
    QSpinBox* m_intervalSpin = nullptr;
    QComboBox* m_intervalTypeCombo = nullptr;

    QLabel* m_titleError = nullptr;
    QLabel* m_dateError = nullptr;
    QLabel* m_intervalError = nullptr;

    QPushButton* m_createButton = nullptr;

    MaintenanceSchedule m_scheduleData;

    QComboBox* m_assetComboBox = nullptr;

    StorageManager& m_storageManager;

   /* QComboBox* m_partsComboBox = nullptr;
    QSpinBox* m_partQuantitySpinBox = nullptr;
    QListWidget* m_plannedPartsList = nullptr;

    QList<MaintenancePart> m_plannedParts;*/
};

#endif