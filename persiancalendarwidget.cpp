#include "persiancalendarwidget.h"
#include "persiandigits.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <qcalendar.h>

namespace
{
const QStringList monthNames =
    {
        "فروردین",
        "اردیبهشت",
        "خرداد",
        "تیر",
        "مرداد",
        "شهریور",
        "مهر",
        "آبان",
        "آذر",
        "دی",
        "بهمن",
        "اسفند"
};

const QStringList weekDays =
    {
        "ش", // شنبه
        "ی", // یکشنبه
        "د", // دوشنبه
        "س", // سه‌شنبه
        "چ", // چهارشنبه
        "پ", // پنجشنبه
        "ج"  // جمعه
};
}

PersianCalendarWidget::PersianCalendarWidget(QWidget* parent)
    : QCalendarWidget(parent)
    , m_jalali(QCalendar::System::Jalali)
{
    setNavigationBarVisible(false);
    setGridVisible(false);

    setHorizontalHeaderFormat(
        QCalendarWidget::NoHorizontalHeader
        );

    setVerticalHeaderFormat(
        QCalendarWidget::NoVerticalHeader
        );

    setFocusPolicy(Qt::StrongFocus);

    setSelectionMode(
        QCalendarWidget::SingleSelection
        );

    setFixedSize(340, 310);

    // Hide QCalendarWidget's internal
    // Gregorian calendar controls.
    const auto children =
        findChildren<QWidget*>();

    for (QWidget* child : children)
        child->hide();

    setPersianDate(QDate::currentDate());
}
QSize PersianCalendarWidget::sizeHint() const
{
    return QSize(340, 310);
}

void PersianCalendarWidget::setPersianDate(const QDate& date)
{
    if (!date.isValid())
        return;

    const auto parts = m_jalali.partsFromDate(date);

    m_year = parts.year;
    m_month = parts.month;

    QCalendarWidget::setSelectedDate(date);

    update();
}

void PersianCalendarWidget::previousMonth()
{
    --m_month;

    if (m_month < 1)
    {
        m_month = 12;
        --m_year;
    }

    update();
}

void PersianCalendarWidget::nextMonth()
{
    ++m_month;

    if (m_month > 12)
    {
        m_month = 1;
        ++m_year;
    }

    update();
}

void PersianCalendarWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect fullRect = rect();

    painter.fillRect(
        fullRect,
        palette().base()
        );

    const int headerHeight = 48;
    const int weekdayHeight = 32;

    // -------------------------
    // Header
    // -------------------------

    QRect headerRect(
        0,
        0,
        width(),
        headerHeight
        );

    painter.setPen(palette().text().color());

    QFont headerFont = font();
    headerFont.setBold(true);
    headerFont.setPointSize(11);

    painter.setFont(headerFont);

    const QString title =
        monthNames[m_month - 1] +
        " " +
        toPersianDigits(
            QString::number(m_year)
            );

    painter.drawText(
        headerRect,
        Qt::AlignCenter,
        title
        );

    // Previous / next buttons

    QFont arrowFont = font();
    arrowFont.setPointSize(16);
    painter.setFont(arrowFont);

    // Previous month
    painter.drawText(
        QRect(8, 0, 45, headerHeight),
        Qt::AlignCenter,
        "‹"
        );

    // Next month
    painter.drawText(
        QRect(width() - 53, 0, 45, headerHeight),
        Qt::AlignCenter,
        "›"
        );

    // -------------------------
    // Weekdays
    // -------------------------

    const int gridTop =
        headerHeight + weekdayHeight;

    const int cellWidth =
        width() / 7;

    painter.setFont(font());

    for (int i = 0; i < 7; ++i)
    {
        // RTL:
        // Saturday is on the right.
        const int visualColumn = 6 - i;

        QRect cell(
            visualColumn * cellWidth,
            headerHeight,
            cellWidth,
            weekdayHeight
            );

        painter.setPen(
            palette().mid().color()
            );

        painter.drawText(
            cell,
            Qt::AlignCenter,
            weekDays[i]
            );
    }

    // -------------------------
    // Days
    // -------------------------

    const QDate firstDate =
        m_jalali.dateFromParts(
            m_year,
            m_month,
            1
            );

    if (!firstDate.isValid())
        return;

    // Qt:
    // Monday = 1
    // ...
    // Saturday = 6
    // Sunday = 7
    //
    // Convert to:
    // Saturday = 0
    // Sunday   = 1
    // Monday   = 2
    // ...
    // Friday   = 6

    const int firstDay =
        (firstDate.dayOfWeek() + 1) % 7;

    int daysInMonth = 0;

    for (int day = 31; day >= 1; --day)
    {
        if (m_jalali.dateFromParts(
                        m_year,
                        m_month,
                        day
                        ).isValid())
        {
            daysInMonth = day;
            break;
        }
    }

    const int gridHeight =
        height() - gridTop;

    const int cellHeight =
        gridHeight / 6;

    const QDate selectedDate =
        QCalendarWidget::selectedDate();

    const QDate today =
        QDate::currentDate();

    QFont dayFont = font();
    dayFont.setPointSize(10);

    painter.setFont(dayFont);

    for (int day = 1; day <= daysInMonth; ++day)
    {
        const int index =
            firstDay + day - 1;

        const int row =
            index / 7;

        const int column =
            index % 7;

        // RTL
        const int visualColumn =
            6 - column;

        QRect cell(
            visualColumn * cellWidth,
            gridTop + row * cellHeight,
            cellWidth,
            cellHeight
            );

        const QDate date =
            m_jalali.dateFromParts(
                m_year,
                m_month,
                day
                );

        // Selected date

        if (date == selectedDate)
        {
            painter.setPen(Qt::NoPen);

            painter.setBrush(
                palette().highlight()
                );

            const QPoint center =
                cell.center();

            painter.drawEllipse(
                center,
                15,
                15
                );

            painter.setPen(
                palette().highlightedText().color()
                );
        }
        else if (date == today)
        {
            painter.setPen(
                palette().highlight().color()
                );
        }
        else
        {
            painter.setPen(
                palette().text().color()
                );
        }

        painter.drawText(
            cell,
            Qt::AlignCenter,
            toPersianDigits(
                QString::number(day)
                )
            );

        const QList<QColor> dots =
            m_eventDots.value(date);

        if (!dots.isEmpty())
        {
            const int dotCount =
                qMin(dots.size(), 3);

            constexpr int dotRadius = 2;
            constexpr int dotSpacing = 7;

            const int totalWidth =
                (dotCount - 1) * dotSpacing;

            const int startX =
                cell.center().x() -
                totalWidth / 2;

            const int y =
                cell.bottom() - 7;

            for (int i = 0; i < dotCount; ++i)
            {
                painter.setPen(Qt::NoPen);

                painter.setBrush(
                    dots[i]
                    );

                painter.drawEllipse(
                    QPoint(
                        startX + i * dotSpacing,
                        y
                        ),
                    dotRadius,
                    dotRadius
                    );
            }
        }
    }
}

void PersianCalendarWidget::mousePressEvent(
    QMouseEvent* event
    )
{
    // Qt5 supported passing position
   // const QPoint pos = event->position().toPoint();
   const QPoint pos = event->localPos().toPoint();

    const int headerHeight = 48;
    const int weekdayHeight = 32;

    // Previous month
    if (QRect(0, 0, 60, headerHeight).contains(pos))
    {
        previousMonth();
        return;
    }

    // Next month
    if (QRect(width() - 60, 0, 60, headerHeight).contains(pos))
    {
        nextMonth();
        return;
    }

    const int gridTop =
        headerHeight + weekdayHeight;

    if (pos.y() < gridTop)
        return;

    const int cellWidth =
        width() / 7;

    const int cellHeight =
        (height() - gridTop) / 6;

    const int visualColumn =
        pos.x() / cellWidth;

    const int column =
        6 - visualColumn;

    const int row =
        (pos.y() - gridTop) / cellHeight;

    const QDate firstDate =
        m_jalali.dateFromParts(
            m_year,
            m_month,
            1
            );

    if (!firstDate.isValid())
        return;

    const int firstDay =
        (firstDate.dayOfWeek() + 1) % 7;

    const int day =
        row * 7 +
        column -
        firstDay +
        1;

    if (day < 1)
        return;

    const QDate selected =
        m_jalali.dateFromParts(
            m_year,
            m_month,
            day
            );

    if (!selected.isValid())
        return;

    QCalendarWidget::setSelectedDate(selected);

    emit clicked(selected);
    emit activated(selected);

    update();
}

void PersianCalendarWidget::setEventDots(
    const QList<CalendarEventDot>& dots)
{
    m_eventDots.clear();

    for (const CalendarEventDot& dot : dots)
    {
        QList<QColor>& colors =
            m_eventDots[dot.date];

        if (!colors.contains(dot.color))
            colors.append(dot.color);

        // Maximum three visible dots.
        if (colors.size() > 4)
            colors.removeLast();
    }

    update();
}