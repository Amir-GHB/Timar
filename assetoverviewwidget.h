#ifndef ASSETOVERVIEWWIDGET_H
#define ASSETOVERVIEWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <quuid.h>
class Asset;

class AssetOverviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssetOverviewWidget(QWidget* parent = nullptr);

    void setAsset(const Asset& asset);

    QUuid m_assetId;

    QUuid assetId() const;

private:
    void setupUI();

    QWidget* createInfoCard(QWidget* parent);
    QWidget* createQuickInfoCard(QWidget* parent);

    QLabel* createFieldLabel(
        const QString& title,
        QWidget* parent
        );
    QWidget* createInfoRow(
        const QString& title,
        QLabel*& valueLabel,
        const QString& iconPath,
        QWidget* parent
        );

    QLabel* m_assetNameLabel = nullptr;
    QLabel* m_assetTypeLabel = nullptr;
    QLabel* m_serialNumberLabel = nullptr;
    QLabel* m_manufacturerLabel = nullptr;
    QLabel* m_modelLabel = nullptr;
    QLabel* m_installationDateLabel = nullptr;
    QLabel* m_installationPlaceLabel = nullptr;
    QLabel* m_descriptionLabel = nullptr;

    QLabel* m_assetImageLabel = nullptr;

    QLabel* m_quickInstallationDateLabel = nullptr;
    QLabel* m_quickInstallationPlaceLabel = nullptr;
    QLabel* m_quickStatusLabel = nullptr;
    QLabel* m_statusLabel = nullptr;

signals:
    void editRequested();
    void deleteRequested(const QUuid& assetId);
};

#endif