#include "persiandigits.h"

#include <QCalendar>
#include <qdatetime.h>
#include <qregularexpression.h>

QString toPersianDigits(const QString& text)
{
    QString result = text;

    result.replace('0', QChar(0x06F0));
    result.replace('1', QChar(0x06F1));
    result.replace('2', QChar(0x06F2));
    result.replace('3', QChar(0x06F3));
    result.replace('4', QChar(0x06F4));
    result.replace('5', QChar(0x06F5));
    result.replace('6', QChar(0x06F6));
    result.replace('7', QChar(0x06F7));
    result.replace('8', QChar(0x06F8));
    result.replace('9', QChar(0x06F9));

    return result;
}

QString toPersianDate(
    const QDate& date,
    const QString& separator
    )
{
    if (!date.isValid())
        return "-";

    const QCalendar jalali(
        QCalendar::System::Jalali
        );

    const QCalendar::YearMonthDay parts =
        jalali.partsFromDate(date);

    const QString year =
        QString::number(parts.year)
            .rightJustified(4, '0');

    const QString month =
        QString::number(parts.month)
            .rightJustified(2, '0');

    const QString day =
        QString::number(parts.day)
            .rightJustified(2, '0');

    const QString result =
        year +
        separator +
        month +
        separator +
        day;

    return toPersianDigits(result);
}
QDate fromPersianDate(
    const QString& date
    )
{
    QString value = date.trimmed();

    // Convert Persian digits to ASCII
    value.replace(QChar(0x06F0), '0');
    value.replace(QChar(0x06F1), '1');
    value.replace(QChar(0x06F2), '2');
    value.replace(QChar(0x06F3), '3');
    value.replace(QChar(0x06F4), '4');
    value.replace(QChar(0x06F5), '5');
    value.replace(QChar(0x06F6), '6');
    value.replace(QChar(0x06F7), '7');
    value.replace(QChar(0x06F8), '8');
    value.replace(QChar(0x06F9), '9');

    const QStringList parts =
        value.split(
            QRegularExpression("[/\\-.]")
            );

    if (parts.size() != 3)
        return {};

    bool okYear = false;
    bool okMonth = false;
    bool okDay = false;

    const int year =
        parts[0].toInt(&okYear);

    const int month =
        parts[1].toInt(&okMonth);

    const int day =
        parts[2].toInt(&okDay);

    if (!okYear || !okMonth || !okDay)
        return {};

    const QCalendar jalali(
        QCalendar::System::Jalali
        );

    return jalali.dateFromParts(
        year,
        month,
        day
        );
}