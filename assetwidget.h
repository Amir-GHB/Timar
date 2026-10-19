#ifndef ASSETWIDGET_H
#define ASSETWIDGET_H

#include <QFrame>
#include <QString>
#include <qdatetime.h>
#include <quuid.h>

class QLabel;

enum class AssetWidgetType
{
    Asset,
    SubAsset
};


class AssetWidget : public QFrame
{
    Q_OBJECT

public:
    explicit AssetWidget(
        const QString& name,
        const QString& serialNumber,
        const QString& imagePath,
        const QDate& installationDate,
        const QString& installationPlace,
        const QString& statusText,
        const QString& statusColor,
        const QUuid& assetId,
        bool isAsset,
        QWidget* parent = nullptr
        );

signals:
    void clicked();
    void editRequested(const QUuid& assetId);
    void deleteRequested(const QUuid& assetId);
    void openRequested(const QUuid& assetId);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    void setupUI();

private:
    QString m_name;
    QString m_serialNumber;
    QString m_imagePath;
    QString m_statusText;
    QString m_statusColor;
    QDate m_installationDate;
    QString m_installationPlace;
    QUuid m_assetId;

    QLabel* m_imageLabel = nullptr;
    QLabel* m_nameLabel = nullptr;
    QLabel* m_serialLabel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_installationDateLabel = nullptr;
    QLabel* m_installationPlaceLabel = nullptr;

    bool m_isAsset = true;

};

#endif