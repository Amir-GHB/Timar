#include "maintenancescheduledialog.h"
#include "persiandateedit.h"
#include "subasset.h"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include "storagemanager.h"
#include <QListWidget>

MaintenanceScheduleDialog::MaintenanceScheduleDialog(
    const QUuid& assetId,
    const QString& assetName,
    const QList<SubAsset>& subAssets,
    StorageManager& storageManager,
    QWidget* parent
    )
    : QDialog(parent)
    , m_assetId(assetId)
    , m_assetName(assetName)
    , m_subAssets(subAssets)
    , m_storageManager(storageManager)
{
    setWindowTitle("ایجاد برنامه تعمیراتی");
    setModal(true);
    setFixedWidth(480);

    setupUI();
}


// ============================================================
// UI
// ============================================================

void MaintenanceScheduleDialog::setupUI()
{
    setLayoutDirection(Qt::RightToLeft);

    auto* mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        25, 20, 25, 20
        );

    mainLayout->setSpacing(12);


    // ========================================================
    // Title
    // ========================================================

    auto* titleLabel =
        new QLabel("عنوان برنامه", this);

    m_titleEdit =
        new QLineEdit(this);

    m_titleEdit->setPlaceholderText(
        "عنوان برنامه را وارد کنید..."
        );

    m_titleError =
        new QLabel("!", this);

    m_titleError->setStyleSheet(
        "color: #e53935;"
        "font-size: 18px;"
        "font-weight: bold;"
        );

    m_titleError->setVisible(false);

    auto* titleRow =
        new QHBoxLayout();

    titleRow->addWidget(m_titleEdit, 1);
    titleRow->addWidget(m_titleError);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(titleRow);


    // ========================================================
    // First Scheduled Date
    // ========================================================

    auto* dateLabel =
        new QLabel("اولین موعد", this);

    m_dateEdit =
        new PersianDateEdit(
            this
            );
    m_dateEdit->setDate(QDate::currentDate());


    m_dateError =
        new QLabel("!", this);

    m_dateError->setStyleSheet(
        "color: #e53935;"
        "font-size: 18px;"
        "font-weight: bold;"
        );

    m_dateError->setVisible(false);

    auto* dateRow =
        new QHBoxLayout();

    dateRow->addWidget(m_dateEdit, 1);
    dateRow->addWidget(m_dateError);

    mainLayout->addWidget(dateLabel);
    mainLayout->addLayout(dateRow);
    //asset/subasset selector
    auto* assetLabel =
        new QLabel("تجهیز", this);

    m_assetComboBox =
        new QComboBox(this);

    m_assetComboBox->addItem(
        m_assetName,
        QVariant::fromValue(QUuid())
        );

    for (const SubAsset& subAsset : m_subAssets)
    {
        m_assetComboBox->addItem(
            "   ↳ " + subAsset.info().name,
            QVariant::fromValue(
                subAsset.id()
                )
            );
    }

    m_assetComboBox->setLayoutDirection(
        Qt::RightToLeft
        );
    // ========================================================
    // Interval
    // ========================================================

    auto* intervalLabel =
        new QLabel("دوره تکرار", this);

    m_intervalSpin =
        new QSpinBox(this);

    m_intervalSpin->setRange(1, 9999);
    m_intervalSpin->setValue(1);

    m_intervalTypeCombo =
        new QComboBox(this);

    m_intervalTypeCombo->addItem("روز");
    m_intervalTypeCombo->addItem("هفته");
    m_intervalTypeCombo->addItem("ماه");
    m_intervalTypeCombo->addItem("سال");

    m_intervalError =
        new QLabel("!", this);

    m_intervalError->setStyleSheet(
        "color: #e53935;"
        "font-size: 18px;"
        "font-weight: bold;"
        );

    m_intervalError->setVisible(false);

    auto* intervalRow =
        new QHBoxLayout();

    intervalRow->addWidget(intervalLabel);
    intervalRow->addWidget(m_intervalSpin);
    intervalRow->addWidget(m_intervalTypeCombo);
    intervalRow->addWidget(m_intervalError);

    mainLayout->addLayout(intervalRow);

    // ========================================================
    // Planned Parts
    // ========================================================
/*
    auto* partsLabel =
        new QLabel("قطعات موردنیاز", this);

    m_partsComboBox =
        new QComboBox(this);

    m_partQuantitySpinBox =
        new QSpinBox(this);

    m_partQuantitySpinBox->setRange(1, 999999);
    m_partQuantitySpinBox->setValue(1);

    auto* addPartButton =
        new QPushButton("افزودن قطعه", this);

    for (const StoragePart& part : m_storageManager.parts())
    {
        if (part.quantity <= 0)
            continue;

        m_partsComboBox->addItem(
            QString("%1 | %2 (موجودی: %3)")
                .arg(part.name)
                .arg(part.partNumber)
                .arg(part.quantity),
            part.id.toString(QUuid::WithoutBraces)
            );
    }

    auto* partInputRow =
        new QHBoxLayout();

    partInputRow->addWidget(m_partsComboBox, 1);
    partInputRow->addWidget(m_partQuantitySpinBox);
    partInputRow->addWidget(addPartButton);

    m_plannedPartsList =
        new QListWidget(this);

    mainLayout->addWidget(partsLabel);
    mainLayout->addLayout(partInputRow);
    mainLayout->addWidget(m_plannedPartsList);

    connect(
        addPartButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const QUuid partId(
                m_partsComboBox->currentData().toString()
                );

            const StoragePart* part =
                m_storageManager.getPart(partId);

            if (!part)
                return;

            const int quantity =
                m_partQuantitySpinBox->value();

            if (quantity > part->quantity)
                return;

            for (MaintenancePart& existing : m_plannedParts)
            {
                if (existing.storagePartId == partId)
                {
                    if (existing.plannedQuantity + quantity >
                        part->quantity)
                        return;

                    existing.plannedQuantity += quantity;

                    for (int i = 0;
                         i < m_plannedPartsList->count();
                         ++i)
                    {
                        auto* item = m_plannedPartsList->item(i);

                        if (QUuid(
                                item->data(Qt::UserRole).toString()
                                ) == partId)
                        {
                            item->setText(
                                QString("%1 | %2 | تعداد: %3")
                                    .arg(part->name)
                                    .arg(part->partNumber)
                                    .arg(existing.plannedQuantity)
                                );
                            break;
                        }
                    }

                    return;
                }
            }

            MaintenancePart plannedPart;
            plannedPart.storagePartId = partId;
            plannedPart.partName = part->name;
            plannedPart.partNumber = part->partNumber;
            plannedPart.plannedQuantity = quantity;
            plannedPart.usedQuantity = 0;

            m_plannedParts.append(plannedPart);

            auto* item = new QListWidgetItem(
                QString("%1 | %2 | تعداد: %3")
                    .arg(part->name)
                    .arg(part->partNumber)
                    .arg(quantity),
                m_plannedPartsList
                );

            item->setData(
                Qt::UserRole,
                partId.toString(QUuid::WithoutBraces)
                );
        }
        );

    auto* removePartButton =
        new QPushButton("حذف قطعه انتخاب‌شده", this);

    mainLayout->addWidget(removePartButton);

    connect(
        removePartButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const int row =
                m_plannedPartsList->currentRow();

            if (row < 0 || row >= m_plannedParts.size())
                return;

            m_plannedParts.removeAt(row);
            delete m_plannedPartsList->takeItem(row);
        }
        );
*/

    // ========================================================
    // Buttons
    // ========================================================

    auto* buttonLayout =
        new QHBoxLayout();

    buttonLayout->addStretch();

    auto* cancelButton =
        new QPushButton("انصراف", this);

    m_createButton =
        new QPushButton("ایجاد برنامه", this);

    m_createButton->setDefault(true);

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(m_createButton);

    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonLayout);


    // ========================================================
    // Connections
    // ========================================================

    connect(
        cancelButton,
        &QPushButton::clicked,
        this,
        &QDialog::reject
        );

    connect(
        m_createButton,
        &QPushButton::clicked,
        this,
        &MaintenanceScheduleDialog::onCreateClicked
        );
}


// ============================================================
// Validation / Creation
// ============================================================

void MaintenanceScheduleDialog::onCreateClicked()
{
    bool valid = true;


    // --------------------------------------------------------
    // Title
    // --------------------------------------------------------

    if (m_titleEdit->text().trimmed().isEmpty())
    {
        m_titleError->setVisible(true);
        valid = false;
    }
    else
    {
        m_titleError->setVisible(false);
    }


    // --------------------------------------------------------
    // Date
    // --------------------------------------------------------

    if (!m_dateEdit->dateTime().isValid())
    {
        m_dateError->setVisible(true);
        valid = false;
    }
    else
    {
        m_dateError->setVisible(false);
    }


    // --------------------------------------------------------
    // Interval
    // --------------------------------------------------------

    if (m_intervalSpin->value() <= 0)
    {
        m_intervalError->setVisible(true);
        valid = false;
    }
    else
    {
        m_intervalError->setVisible(false);
    }


    if (!valid)
        return;


    // ========================================================
    // Create schedule data
    // ========================================================

    m_scheduleData =
        MaintenanceSchedule();
    m_scheduleData.assetId =
        m_assetId;

    m_scheduleData.id =
        QUuid::createUuid();

    m_scheduleData.title =
        m_titleEdit->text().trimmed();

    m_scheduleData.nextScheduledDate =
        m_dateEdit->date();

    m_scheduleData.interval =
        m_intervalSpin->value();


    switch (m_intervalTypeCombo->currentIndex())
    {
    case 0:
        m_scheduleData.intervalType =
            MaintenanceIntervalType::Days;
        break;

    case 1:
        m_scheduleData.intervalType =
            MaintenanceIntervalType::Weeks;
        break;

    case 2:
        m_scheduleData.intervalType =
            MaintenanceIntervalType::Months;
        break;

    case 3:
        m_scheduleData.intervalType =
            MaintenanceIntervalType::Years;
        break;
    }

    accept();
}


// ============================================================
// Result
// ============================================================

MaintenanceSchedule
MaintenanceScheduleDialog::scheduleData() const
{
    MaintenanceSchedule schedule;

    schedule.id = QUuid::createUuid();

    schedule.assetId = m_assetId;

    schedule.subAssetId =
        m_assetComboBox->currentData()
            .toUuid();

    schedule.title =
        m_titleEdit->text().trimmed();

    schedule.nextScheduledDate =
        m_dateEdit->date();

    schedule.interval =
        m_intervalSpin->value();

    schedule.intervalType =
        static_cast<MaintenanceIntervalType>(
            m_intervalTypeCombo->currentIndex()
            );

    schedule.status =
        MaintenanceScheduleStatus::Active;

    //schedule.plannedParts = m_plannedParts;

    return schedule;
}