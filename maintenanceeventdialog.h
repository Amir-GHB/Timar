#ifndef MAINTENANCEEVENTDIALOG_H
#define MAINTENANCEEVENTDIALOG_H

#include "maintenanceevent.h"
#include "maintenanceschedule.h"
#include "persiandateedit.h"

#include <QDialog>
#include <QList>

class QLineEdit;
class QTextEdit;
class QDateEdit;
class QCheckBox;
class QLabel;
class QPushButton;
class QVBoxLayout;

class MaintenanceEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaintenanceEventDialog(
        const QString& assetName,
        const QString& subAssetName,
        const QList<MaintenanceSchedule>& schedules,
        QWidget* parent = nullptr
        );

    MaintenanceEventData eventData() const;

private slots:
    void onCreateClicked();
    void onDeadlineToggled(bool checked);
    void onAddScheduleClicked();

private:
    void setupUI();

    void addAffectedSchedule(
        const MaintenanceSchedule& schedule
        );

    void removeAffectedSchedule(
        const QUuid& scheduleId
        );

    void refreshAffectedScheduleList();

private:
    QString m_assetName;
    QString m_subAssetName;

    QList<MaintenanceSchedule> m_schedules;

    QList<QUuid> m_affectedScheduleIds;

    QLineEdit* m_titleEdit = nullptr;
    QTextEdit* m_descriptionEdit = nullptr;

    QCheckBox* m_hasDeadlineCheck = nullptr;
    PersianDateEdit* m_dateEdit = nullptr;

    QLabel* m_titleError = nullptr;
    QLabel* m_dateError = nullptr;

    QPushButton* m_addScheduleButton = nullptr;

    QWidget* m_affectedSchedulesWidget = nullptr;
    QVBoxLayout* m_affectedSchedulesLayout = nullptr;

    QPushButton* m_createButton = nullptr;

    MaintenanceEventData m_eventData;
};

#endif