#pragma once

#include <QUuid>
#include <QString>

struct StoragePart
{
    QUuid id;
    QString name;
    QString partNumber;
    QString description;
    int quantity = 0;
};