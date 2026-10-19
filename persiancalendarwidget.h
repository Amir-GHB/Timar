#include <QHash>
#include <QList>
#include <qcalendar.h>
#include <qcalendarwidget.h>

struct CalendarEventDot
{
    QDate date;
    QColor color;
};

class PersianCalendarWidget : public QCalendarWidget
{
    Q_OBJECT

public:
    explicit PersianCalendarWidget(QWidget* parent = nullptr);

    void setPersianDate(const QDate& date);

    void setEventDots(
        const QList<CalendarEventDot>& dots
        );

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void previousMonth();
    void nextMonth();

    QCalendar m_jalali;

    int m_year = 1400;
    int m_month = 1;

    QHash<QDate, QList<QColor>> m_eventDots;
};