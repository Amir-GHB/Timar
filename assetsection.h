#ifndef ASSETSECTION_H
#define ASSETSECTION_H

#include "asset.h"
#include <QWidget>
#include <QUuid>
#include <qboxlayout.h>

class Asset;
class AssetManager;
class AssetWidget;

class QLineEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;

class AssetSection : public QWidget
{
    Q_OBJECT

public:
    explicit AssetSection(
        AssetManager& assetManager,
        QWidget* parent = nullptr
        );

    void refreshAssets();

signals:
    void assetSelected(const QUuid& assetId);
    void addAssetRequested(const AssetInfo& info);

    void editAssetRequested(const QUuid& assetId);
    void deleteAssetRequested(const QUuid& assetId);
    void openAssetRequested(const QUuid& assetId);

private:
    void setupUI();

    void rebuildTree();

    QTreeWidgetItem* createAssetItem(
        const Asset& asset
        );

    void filterTree(
        const QString& text
        );

    bool filterItem(
        QTreeWidgetItem* item,
        const QString& text
        );

    void selectAsset(
        const QUuid& assetId
        );

    void updateInfoPanel(
        const Asset& asset
        );

    QWidget* createInfoItem(
        const QString& iconPath,
        const QString& title,
        const QString& value
        );

    QWidget* createDescriptionItem(
        const QString& iconPath,
        const QString& label,
        const QString& value
        );

private:
    AssetManager& m_assetManager;

    QLineEdit* m_searchEdit = nullptr;
    QPushButton* m_addButton = nullptr;

    QTreeWidget* m_assetTree = nullptr;

    QLabel* m_infoImageLabel = nullptr;
    QLabel* m_infoNameLabel = nullptr;
    QLabel* m_infoTypeLabel = nullptr;

    QWidget* m_infoPanel = nullptr;
    QVBoxLayout* m_infoLayout = nullptr;
    QVBoxLayout* m_infoItemsLayout = nullptr;

    QUuid m_selectedAssetId;

};

#endif