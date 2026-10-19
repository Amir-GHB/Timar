#ifndef SUBASSET_H
#define SUBASSET_H

#include <QString>
#include <QUuid>
#include <QDate>

struct SubAssetInfo
{
    QString name;
    QString serialNumber;
    QString manufacturer;
    QString model;
    QString type;

    QDate installationDate;

    QString installationPlace;

    QString imagePath;

    QString description;

    QString status;
};

class SubAsset
{
public:
    SubAsset();

    explicit SubAsset(
        const SubAssetInfo& info
    );

    SubAsset(
        const QUuid& id,
        const SubAssetInfo& info
    );

    QUuid id() const;

    const SubAssetInfo& info() const;

    void setInfo(
        const SubAssetInfo& info
    );

private:
    QUuid m_id;
    SubAssetInfo m_info;
};

#endif