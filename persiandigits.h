#pragma once

#include <QDate>
#include <QString>

QString toPersianDigits(const QString& text);

QString toPersianDate(
    const QDate& date,
    const QString& separator = "/"
    );

QDate fromPersianDate(
    const QString& date
    );