#ifndef MAINTENANCEEVENTWIDGET_H
#define MAINTENANCEEVENTWIDGET_H

#include "maintenanceevent.h"

#include <QWidget>
#include <qframe.h>

class QLabel;
class QPushButton;

class MaintenanceEventWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MaintenanceEventWidget(
        const MaintenanceEventData& eventData,
        QWidget* parent = nullptr
        );

signals:
    void statusChangeRequested(
        const QUuid& eventId,
        MaintenanceStatus newStatus
        );

    void editRequested(
        const QUuid& eventId
        );

    void deleteRequested(
        const QUuid& eventId
        );

private slots:
    void showActionMenu();

    void onCompleteClicked();
    void onWaitingForHandlingClicked();
    void onEditClicked();
    void onCancelClicked();
    void onDeleteClicked();

private:
    void setupUI();
    void updateUI();
    void updateBackground();
    void updateStatus();

    QWidget* createInfoItem(
        const QString& iconPath,
        const QString& title,
        QLabel*& valueLabel
        );

    MaintenanceEventData m_eventData;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_typeLabel = nullptr;

    QLabel* m_assetLabel = nullptr;
    QLabel* m_subAssetLabel = nullptr;
    QLabel* m_placeLabel = nullptr;
    QLabel* m_dateLabel = nullptr;

    QLabel* m_statusLabel = nullptr;

    QPushButton* m_menuButton = nullptr;

    QFrame* m_actionMenu = nullptr;

    QPushButton* m_completeButton = nullptr;
    QPushButton* m_waitingButton = nullptr;
    QPushButton* m_editButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    QPushButton* m_deleteButton = nullptr;

    void setupActionMenu();
    void hideActionMenu();

    QFrame* createInfoSeparator(QWidget* parent);
};

#endif // MAINTENANCEEVENTWIDGET_H