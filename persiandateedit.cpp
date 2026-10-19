#include "persiandateedit.h"
#include "persiancalendarwidget.h"
#include "persiandateedit.h"
#include "persiandigits.h"

#include <QLineEdit>

PersianDateEdit::PersianDateEdit(QWidget* parent)
    : QDateEdit(parent)
{
    setCalendarPopup(true);

    setDisplayFormat("yyyy/MM/dd");

    // We want selection through the Persian calendar.
    lineEdit()->setReadOnly(true);

    m_calendar =
        new PersianCalendarWidget(this);

    setCalendarWidget(m_calendar);

    connect(
        this,
        &QDateEdit::dateChanged,
        this,
        [this](const QDate& date)
        {
            m_calendar->setPersianDate(date);
        }
        );

    connect(
        m_calendar,
        &QCalendarWidget::clicked,
        this,
        [this](const QDate& date)
        {
            setDate(date);
        }
        );

    m_calendar->setPersianDate(date());
}

void PersianDateEdit::paintEvent(QPaintEvent* event)
{
    QDateEdit::paintEvent(event);

    if (lineEdit())
    {
        lineEdit()->setText(
            toPersianDate(date())
            );
    }
}