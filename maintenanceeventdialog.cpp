#include "maintenanceeventdialog.h"

#include <QCheckBox>
#include "persiandateedit.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <qmenu.h>

MaintenanceEventDialog::MaintenanceEventDialog(
    const QString& assetName,
    const QString& subAssetName,
    const QList<MaintenanceSchedule>& schedules,
    QWidget* parent
    )
    : QDialog(parent)
    , m_assetName(assetName)
    , m_subAssetName(subAssetName)
    , m_schedules(schedules)
{
    setWindowTitle("ایجاد رویداد");
    setModal(true);
    setFixedWidth(480);

    setupUI();
}


// ============================================================
// UI
// ============================================================

void MaintenanceEventDialog::setupUI()
{
    setLayoutDirection(Qt::RightToLeft);

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(12);


    // ========================================================
    // Title
    // ========================================================

    auto* titleLabel =
        new QLabel("عنوان رویداد", this);

    m_titleEdit =
        new QLineEdit(this);

    m_titleEdit->setPlaceholderText(
        "عنوان رویداد را وارد کنید..."
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
    // Description
    // ========================================================

    auto* descriptionLabel =
        new QLabel("توضیحات", this);

    m_descriptionEdit =
        new QTextEdit(this);

    m_descriptionEdit->setPlaceholderText(
        "توضیحات اختیاری..."
        );

    m_descriptionEdit->setFixedHeight(80);

    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(m_descriptionEdit);


    // ========================================================
    // Deadline
    // ========================================================

    auto* deadlineLabel =
        new QLabel("مهلت انجام", this);

    m_hasDeadlineCheck =
        new QCheckBox("دارای مهلت", this);

    m_hasDeadlineCheck->setChecked(true);

    m_dateEdit = new PersianDateEdit(this);
    m_dateEdit->setDate(QDate::currentDate());


    m_dateError =
        new QLabel("!", this);

    m_dateError->setStyleSheet(
        "color: #e53935;"
        "font-size: 18px;"
        "font-weight: bold;"
        );

    m_dateError->setVisible(false);

    auto* deadlineRow =
        new QHBoxLayout();

    deadlineRow->addWidget(m_dateEdit, 1);
    deadlineRow->addWidget(m_hasDeadlineCheck);
    deadlineRow->addWidget(m_dateError);

    mainLayout->addWidget(deadlineLabel);
    mainLayout->addLayout(deadlineRow);


    // ========================================================
    // Affected Schedules
    // ========================================================

    auto* affectedSchedulesLabel =
        new QLabel("برنامه‌های تعمیراتی مؤثر", this);

    mainLayout->addWidget(
        affectedSchedulesLabel
        );


    // --------------------------------------------------------
    // Add button
    // --------------------------------------------------------

    m_addScheduleButton =
        new QPushButton(
            "+ افزودن برنامه",
            this
            );

    m_addScheduleButton->setFixedHeight(34);

    mainLayout->addWidget(
        m_addScheduleButton
        );


    // --------------------------------------------------------
    // Selected schedules
    // --------------------------------------------------------

    m_affectedSchedulesWidget =
        new QWidget(this);

    m_affectedSchedulesLayout =
        new QVBoxLayout(
            m_affectedSchedulesWidget
            );

    m_affectedSchedulesLayout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    m_affectedSchedulesLayout->setSpacing(5);

    mainLayout->addWidget(
        m_affectedSchedulesWidget
        );
    // ========================================================
    // Buttons
    // ========================================================

    auto* buttonLayout =
        new QHBoxLayout();

    buttonLayout->addStretch();

    auto* cancelButton =
        new QPushButton("انصراف", this);

    m_createButton =
        new QPushButton("ایجاد رویداد", this);

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
        &MaintenanceEventDialog::onCreateClicked
        );

    connect(
        m_hasDeadlineCheck,
        &QCheckBox::toggled,
        this,
        &MaintenanceEventDialog::onDeadlineToggled
        );

    connect(
        m_addScheduleButton,
        &QPushButton::clicked,
        this,
        &MaintenanceEventDialog::onAddScheduleClicked
        );
}


// ============================================================
// Deadline
// ============================================================

void MaintenanceEventDialog::onDeadlineToggled(
    bool checked
    )
{
    m_dateEdit->setEnabled(checked);
    m_dateError->setVisible(false);
}


// ============================================================
// Validation / Creation
// ============================================================

void MaintenanceEventDialog::onCreateClicked()
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
    // Deadline
    // --------------------------------------------------------

    if (m_hasDeadlineCheck->isChecked() &&
        !m_dateEdit->dateTime().isValid())
    {
        m_dateError->setVisible(true);
        valid = false;
    }
    else
    {
        m_dateError->setVisible(false);
    }


    if (!valid)
        return;


    // ========================================================
    // Create event data
    // ========================================================

    m_eventData = MaintenanceEventData();

    m_eventData.id =
        QUuid::createUuid();

    m_eventData.title =
        m_titleEdit->text().trimmed();

    m_eventData.description =
        m_descriptionEdit->toPlainText().trimmed();

    m_eventData.assetName =
        m_assetName;

    m_eventData.subAssetName =
        m_subAssetName;

    m_eventData.type =
        MaintenanceType::EM;

    // Manual event = no schedule
    m_eventData.scheduleId =
        QUuid();

    m_eventData.affectedScheduleIds =
        m_affectedScheduleIds;

    if (m_hasDeadlineCheck->isChecked())
    {
        m_eventData.scheduledDate =
            m_dateEdit->date();

        m_eventData.status =
            MaintenanceStatus::Scheduled;
    }
    else
    {
        m_eventData.scheduledDate =
            QDate();

        m_eventData.status =
            MaintenanceStatus::WaitingForHandling;
    }

    accept();
}


// ============================================================
// Result
// ============================================================

MaintenanceEventData
MaintenanceEventDialog::eventData() const
{
    return m_eventData;
}

void MaintenanceEventDialog::onAddScheduleClicked()
{
    auto* menu = new QMenu(this);

    for (const MaintenanceSchedule& schedule : m_schedules)
    {
        if (m_affectedScheduleIds.contains(schedule.id))
            continue;

        QAction* action =
            menu->addAction(schedule.title);

        connect(
            action,
            &QAction::triggered,
            this,
            [this, schedule]()
            {
                addAffectedSchedule(schedule);
            }
            );
    }

    if (menu->actions().isEmpty())
    {
        QAction* action =
            menu->addAction("برنامه‌ای برای افزودن وجود ندارد");

        action->setEnabled(false);
    }

    menu->exec(
        m_addScheduleButton->mapToGlobal(
            QPoint(
                0,
                m_addScheduleButton->height()
                )
            )
        );

    menu->deleteLater();
}

void MaintenanceEventDialog::addAffectedSchedule(
    const MaintenanceSchedule& schedule
    )
{
    if (m_affectedScheduleIds.contains(schedule.id))
        return;

    m_affectedScheduleIds.append(
        schedule.id
        );

    refreshAffectedScheduleList();
}

void MaintenanceEventDialog::refreshAffectedScheduleList()
{
    while (
        QLayoutItem* item =
        m_affectedSchedulesLayout->takeAt(0)
        )
    {
        if (QWidget* widget = item->widget())
            widget->deleteLater();

        delete item;
    }


    for (const QUuid& scheduleId :
         m_affectedScheduleIds)
    {
        QString title;

        for (const MaintenanceSchedule& schedule :
             m_schedules)
        {
            if (schedule.id == scheduleId)
            {
                title = schedule.title;
                break;
            }
        }

        if (title.isEmpty())
            continue;


        auto* row =
            new QWidget(
                m_affectedSchedulesWidget
                );

        auto* layout =
            new QHBoxLayout(row);

        layout->setContentsMargins(
            0,
            0,
            0,
            0
            );

        layout->setSpacing(6);


        auto* removeButton =
            new QPushButton("×", row);

        removeButton->setFixedSize(
            28,
            28
            );

        removeButton->setStyleSheet(
            "QPushButton {"
            "    color: #e53935;"
            "    font-size: 18px;"
            "    font-weight: bold;"
            "    border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: #ffebee;"
            "}"
            );


        auto* titleLabel =
            new QLabel(title, row);

        titleLabel->setAlignment(
            Qt::AlignRight |
            Qt::AlignVCenter
            );


        layout->addWidget(
            removeButton
            );

        layout->addWidget(
            titleLabel,
            1
            );


        m_affectedSchedulesLayout->addWidget(
            row
            );


        const QUuid id = scheduleId;

        connect(
            removeButton,
            &QPushButton::clicked,
            this,
            [this, id]()
            {
                removeAffectedSchedule(id);
            }
            );
    }
}

void MaintenanceEventDialog::removeAffectedSchedule(
    const QUuid& scheduleId
    )
{
    m_affectedScheduleIds.removeAll(
        scheduleId
        );

    refreshAffectedScheduleList();
}