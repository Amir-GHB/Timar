#ifndef SUBASSETSWIDGET_H
#define SUBASSETSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>

#include "asset.h"

class SubAssetsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubAssetsWidget(QWidget* parent = nullptr);

    void setAsset(const Asset& asset);

private:
    void setupUI();

    void clearSubAssetItems();

    void showSubAsset(
        const QUuid& subAssetId
        );

    QWidget* createListPanel(
        QWidget* parent
        );

    QWidget* createDetailPanel(
        QWidget* parent
        );

    QWidget* createInfoRow(
        const QString& title,
        QLabel*& valueLabel,
        QWidget* parent
        );

    QLabel* createFieldLabel(
        const QString& title,
        QWidget* parent
        );

signals:

    void editSubAssetRequested(
        const QUuid& assetId,
        const QUuid& subAssetId,
        const SubAssetInfo& info
        );

    void deleteSubAssetRequested(
        const QUuid& assetId,
        const QUuid& subAssetId
        );

private:
    // Current asset
    Asset m_currentAsset;

    QUuid m_selectedSubAssetId;

    // Main layout
    QVBoxLayout* m_listLayout = nullptr;

    // Detail panel
    QLabel* m_imageLabel = nullptr;

    QLabel* m_nameLabel = nullptr;
    QLabel* m_statusLabel = nullptr;

    QLabel* m_typeLabel = nullptr;
    QLabel* m_serialNumberLabel = nullptr;
    QLabel* m_manufacturerLabel = nullptr;
    QLabel* m_modelLabel = nullptr;
    QLabel* m_installationDateLabel = nullptr;
    QLabel* m_installationPlaceLabel = nullptr;
    QLabel* m_descriptionLabel = nullptr;

    // List items
    QList<QWidget*> m_items;

    QUuid m_assetId;

signals:
    void addSubAssetRequested(
        const QUuid& assetId,
        const SubAssetInfo& info
        );
};

#endif