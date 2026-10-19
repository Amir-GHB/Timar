#include "timelinewidget.h"

#include "maintenancemanager.h"
#include "persiancalendarwidget.h"
#include "persiandigits.h"

#include <QCalendar>
#include <QCalendarWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <qapplication.h>
#include <qevent.h>
#include <qheaderview.h>
#include <qscrollbar.h>
#include <qtableview.h>

class TimelineEventFrame : public QFrame
{
public:
    explicit TimelineEventFrame(QWidget* parent = nullptr)
        : QFrame(parent)
    {
        setCursor(Qt::PointingHandCursor);
    }

    std::function<void()> onClicked;

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        QFrame::mousePressEvent(event);

        if (
            event->button() == Qt::LeftButton &&
            onClicked
            )
        {
            auto callback = onClicked;

            QTimer::singleShot(
                0,
                this,
                [callback]()
                {
                    callback();
                }
                );
        }
    }
};

TimelineWidget::TimelineWidget(
    MaintenanceManager* maintenanceManager,
    AssetManager* assetManager,
    QWidget* parent
    )
    : QWidget(parent)
    , m_maintenanceManager(maintenanceManager)
    , m_assetManager(assetManager)
{
    setLayoutDirection(Qt::RightToLeft);

    setupUI();

  if (m_maintenanceManager)
    {
        connect(
            m_maintenanceManager,
            &MaintenanceManager::eventsUpdated,
            this,
            &TimelineWidget::refresh
            );
    }

    refresh();
}


// ============================================================
// UI
// ============================================================

void TimelineWidget::setupUI()
{
    auto* mainLayout =
        new QHBoxLayout(this);

    mainLayout->setContentsMargins(
        20, 20, 20, 20
        );

    mainLayout->setSpacing(16);

    setupTimeline();
    setupRightPanel();

    // Split available width equally between
    // timeline and right/calendar panel.
    mainLayout->setStretch(0, 13);
    mainLayout->setStretch(1, 7);
}


// ============================================================
// Timeline
// ============================================================

void TimelineWidget::setupTimeline()
{
    auto* timelineContainer =
        new QWidget(this);

    timelineContainer->setLayoutDirection(
        Qt::RightToLeft
        );

    auto* layout =
        new QVBoxLayout(timelineContainer);

    layout->setContentsMargins(
        0, 0, 0, 0
        );

    layout->setSpacing(12);

    // --------------------------------------------------------
    // Header
    // --------------------------------------------------------

    auto* title =
        new QLabel(
            "تاریخچه رویدادها",
            timelineContainer
            );

    title->setStyleSheet(
        "font-size: 22px;"
        "font-weight: 700;"
        "color: #172554;"
        );

    auto* description =
        new QLabel(
            "نمایش تمامی رویدادهای مرتبط با تجهیزات و زیرتجهیزات",
            timelineContainer
            );

    description->setStyleSheet(
        "font-size: 13px;"
        "color: #64748b;"
        );

    layout->addWidget(title);
    layout->addWidget(description);

    // --------------------------------------------------------
    // Scroll area
    // --------------------------------------------------------

    m_scrollArea =
        new QScrollArea(timelineContainer);

    m_scrollArea->setWidgetResizable(true);

    m_scrollArea->setFrameShape(
        QFrame::NoFrame
        );

    m_scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    auto* container =
        new QWidget();

    m_timelineLayout =
        new QVBoxLayout(container);

    m_timelineLayout->setContentsMargins(
        0, 16, 10, 10
        );

    m_timelineLayout->setSpacing(10);

    connect(
        m_scrollArea->verticalScrollBar(),
        &QScrollBar::valueChanged,
        this,
        [this]()
        {
            checkScrollPosition();
        }
        );

    m_scrollArea->setWidget(
        container
        );

    layout->addWidget(
        m_scrollArea,
        1
        );

    m_nextEventButton =
        new QPushButton(
            "↓  رفتن به رویداد بعدی",
            m_scrollArea->viewport()
            );

    m_nextEventButton->setCursor(
        Qt::PointingHandCursor
        );

    m_nextEventButton->setFixedHeight(38);

    m_nextEventButton->setStyleSheet(
        "QPushButton {"
        "    background: #2563eb;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 19px;"
        "    padding: 0 18px;"
        "    font-size: 12px;"
        "    font-weight: 700;"
        "}"
        "QPushButton:hover {"
        "    background: #1d4ed8;"
        "}"
        );

    connect(
        m_nextEventButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const QDate nextDate =
                findNextEventDate();

            if (!nextDate.isValid())
                return;

            if (nextDate < m_loadedStartDate)
            {
                const int days =
                    m_loadedStartDate.daysTo(nextDate);

                prependDays(
                    ((days / 7) + 1) * 7
                    );
            }
            else if (nextDate > m_loadedEndDate)
            {
                const int days =
                    m_loadedEndDate.daysTo(nextDate);

                appendDays(
                    ((days / 7) + 1) * 7
                    );
            }

            scrollToDate(nextDate);
        }
        );

    //loading indicator

    m_loadingIndicator =
        new QWidget(m_scrollArea->viewport());

    m_loadingIndicator->setFixedSize(
        190,
        46
        );

    m_loadingIndicator->setStyleSheet(
        "QWidget {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 23px;"
        "}"
        );

    auto* loadingLayout =
        new QHBoxLayout(m_loadingIndicator);

    loadingLayout->setContentsMargins(
        14, 4, 14, 4
        );

    loadingLayout->setSpacing(8);

    m_loadingLabel =
        new QLabel(
            "در حال بارگذاری",
            m_loadingIndicator
            );

    m_loadingLabel->setStyleSheet(
        "font-size: 11px;"
        "font-weight: 600;"
        "color: #64748b;"
        "border: none;"
        "background: transparent;"
        );

    loadingLayout->addWidget(
        m_loadingLabel
        );

    m_loadingIndicator->hide();

    m_loadingAnimationTimer =
        new QTimer(this);

    m_loadingAnimationTimer->setInterval(120);

    connect(
        m_loadingAnimationTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            if (!m_loadingLabel)
                return;

            static const QStringList frames =
                {
                    "در حال بارگذاری   ",
                    "در حال بارگذاری . ",
                    "در حال بارگذاری ..",
                    "در حال بارگذاری ..."
                };

            m_loadingLabel->setText(
                frames[m_loadingAnimationFrame %
                       frames.size()]
                );

            ++m_loadingAnimationFrame;
        }
        );

    m_loadingIndicator->move(
        (m_scrollArea->viewport()->width()
         - m_loadingIndicator->width()) / 2,

        m_scrollArea->viewport()->height()
            - m_loadingIndicator->height()
            - 12
        );

    m_loadingIndicator->raise();

    connect(
        m_scrollArea->viewport(),
        &QWidget::customContextMenuRequested,
        this,
        [](const QPoint&)
        {
        }
        );

    m_nextEventButton->hide();

    auto* mainLayout =
        qobject_cast<QHBoxLayout*>(this->layout());

    if (mainLayout)
    {
        mainLayout->addWidget(
            timelineContainer,
            1
            );
    }
}


// ============================================================
// Right panel
// ============================================================

void TimelineWidget::setupRightPanel()
{
    auto* rightPanel =
        new QFrame(this);

    rightPanel->setMinimumWidth(300);

    rightPanel->setStyleSheet(
        "QFrame#timelineRightPanel {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 12px;"
        "}"
        );

    rightPanel->setObjectName(
        "timelineRightPanel"
        );

    auto* layout =
        new QVBoxLayout(rightPanel);

    layout->setContentsMargins(
        14, 14, 14, 14
        );

    layout->setSpacing(12);

    // ========================================================
    // Asset information
    // ========================================================

    auto* assetFrame =
        new QFrame(rightPanel);

    assetFrame->setStyleSheet(
        "QFrame {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 10px;"
        "}"
        );

    auto* assetLayout =
        new QVBoxLayout(assetFrame);

    assetLayout->setContentsMargins(
        10, 10, 10, 10
        );

    assetLayout->setSpacing(8);

    // --------------------------------------------------------
    // Image + name
    // --------------------------------------------------------

    auto* assetTopLayout =
        new QHBoxLayout();

    assetTopLayout->setSpacing(10);

    m_assetImageLabel =
        new QLabel(assetFrame);

    m_assetImageLabel->setFixedSize(
        72, 72
        );

    m_assetImageLabel->setAlignment(
        Qt::AlignCenter
        );

    m_assetImageLabel->setStyleSheet(
        "background: #f1f5f9;"
        "border-radius: 8px;"
        );

    auto* assetTextLayout =
        new QVBoxLayout();

    assetTextLayout->setSpacing(3);

    m_assetNameLabel =
        new QLabel(
            "تجهیز انتخاب نشده",
            assetFrame
            );

    m_assetNameLabel->setWordWrap(true);

    m_assetNameLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 700;"
        "color: #172554;"
        "border: none;"
        );

    m_assetTypeLabel =
        new QLabel(
            "",
            assetFrame
            );

    m_assetTypeLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #64748b;"
        "border: none;"
        );

    assetTextLayout->addWidget(
        m_assetNameLabel
        );

    assetTextLayout->addWidget(
        m_assetTypeLabel
        );

    assetTextLayout->addStretch();

    assetTopLayout->addWidget(
        m_assetImageLabel
        );

    assetTopLayout->addLayout(
        assetTextLayout,
        1
        );

    assetLayout->addLayout(
        assetTopLayout
        );

    // --------------------------------------------------------
    // Serial
    // --------------------------------------------------------

    m_assetSerialLabel =
        new QLabel(
            "",
            assetFrame
            );

    m_assetSerialLabel->setStyleSheet(
        "font-size: 11px;"
        "color: #64748b;"
        "border: none;"
        );

    assetLayout->addWidget(
        m_assetSerialLabel
        );

    layout->addWidget(
        assetFrame
        );

    // ========================================================
    // Quick statistics
    // ========================================================

    auto* statsFrame =
        new QFrame(rightPanel);

    statsFrame->setStyleSheet(
        "QFrame {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 10px;"
        "}"
        );

    auto* statsLayout =
        new QHBoxLayout(statsFrame);

    statsLayout->setContentsMargins(
        4, 10, 4, 10
        );

    statsLayout->setSpacing(2);

    auto createStat =
        [statsFrame](
            const QString& title,
            QLabel*& valueLabel
            )
    {
        auto* container =
            new QWidget(statsFrame);

        auto* layout =
            new QVBoxLayout(container);

        layout->setContentsMargins(
            4, 0, 4, 0
            );

        layout->setSpacing(2);

        auto* titleLabel =
            new QLabel(
                title,
                container
                );

        titleLabel->setAlignment(
            Qt::AlignCenter
            );

        titleLabel->setStyleSheet(
            "font-size: 10px;"
            "color: #64748b;"
            );

        valueLabel =
            new QLabel(
                "0",
                container
                );

        valueLabel->setAlignment(
            Qt::AlignCenter
            );

        valueLabel->setStyleSheet(
            "font-size: 18px;"
            "font-weight: 700;"
            "color: #172554;"
            );

        layout->addWidget(
            titleLabel
            );

        layout->addWidget(
            valueLabel
            );

        return container;
    };

    statsLayout->addWidget(
        createStat(
            "کل رویدادها",
            m_totalEventsLabel
            )
        );

    statsLayout->addWidget(
        createStat(
            "انجام شده",
            m_completedEventsLabel
            )
        );

    statsLayout->addWidget(
        createStat(
            "تاخیر خورده",
            m_overdueEventsLabel
            )
        );

    layout->addWidget(
        statsFrame
        );

    // ========================================================
    // Calendar
    // ========================================================

    auto* calendarFrame =
        new QFrame(rightPanel);

    calendarFrame->setStyleSheet(
        "QFrame {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 10px;"
        "}"
        );

    auto* calendarLayout =
        new QVBoxLayout(calendarFrame);

    calendarLayout->setContentsMargins(
        6, 8, 6, 8
        );

    calendarLayout->setSpacing(4);

    auto* calendarTitle =
        new QLabel(
            "نمایش تقویم",
            calendarFrame
            );

    calendarTitle->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 700;"
        "color: #172554;"
        "border: none;"
        );

    calendarLayout->addWidget(
        calendarTitle
        );

    m_calendarWidget =new PersianCalendarWidget(calendarFrame);

    connect(
        m_calendarWidget,
        &PersianCalendarWidget::clicked,
        this,
        &TimelineWidget::showCalendarDate
        );

    auto* calendarView =
        m_calendarWidget->findChild<QTableView*>();

    if (calendarView)
    {
        calendarView->horizontalHeader()->setDefaultSectionSize(24);
    }
    if (calendarView)
    {
        calendarView->setStyleSheet(
            "QAbstractItemView {"
            "    font-size: 10px;"
            "    outline: none;"
            "}"
            ""
            "QAbstractItemView::item {"
            "    padding-left: 0px;"
            "    padding-right: 0px;"
            "}"
            );
    }

    calendarLayout->addWidget(
        m_calendarWidget
        );

    layout->addWidget(
        calendarFrame
        );

    // ========================================================
    // Legend
    // ========================================================

    layout->addWidget(
        createStatusLegend()
        );

    layout->addStretch();

    auto* parentLayout =
        qobject_cast<QHBoxLayout*>(this->layout());

    if (parentLayout)
    {
        /*
         * Right side of the main timeline.
         *
         * Because the entire widget is RTL this will visually
         * appear on the right.
         */
        parentLayout->addWidget(
            rightPanel
            );
    }
}


// ============================================================
// Status legend
// ============================================================

QWidget* TimelineWidget::createStatusLegend()
{
    auto* frame =
        new QFrame(this);

    frame->setStyleSheet(
        "QFrame {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 10px;"
        "}"
        );

    auto* layout =
        new QVBoxLayout(frame);

    layout->setContentsMargins(
        12, 12, 12, 12
        );

    layout->setSpacing(8);

    auto* title =
        new QLabel(
            "راهنمای وضعیت‌ها",
            frame
            );

    title->setStyleSheet(
        "font-size: 13px;"
        "font-weight: 700;"
        "color: #172554;"
        "border: none;"
        );

    layout->addWidget(title);

    const QList<MaintenanceStatus> statuses =
        {
            MaintenanceStatus::Completed,
            MaintenanceStatus::Overdue,
            MaintenanceStatus::Missed,
            MaintenanceStatus::Scheduled
        };

    for (const MaintenanceStatus status : statuses)
    {
        auto* row =
            new QHBoxLayout();

        auto* dot =
            new QLabel(frame);

        dot->setFixedSize(10, 10);

        dot->setStyleSheet(
            QString(
                "background: %1;"
                "border-radius: 5px;"
                "border: none;"
                )
                .arg(statusColor(status))
            );

        auto* label =
            new QLabel(
                statusText(status),
                frame
                );

        label->setStyleSheet(
            "font-size: 11px;"
            "color: #64748b;"
            "border: none;"
            );

        row->addWidget(
            dot
            );

        row->addWidget(
            label,
            1
            );

        layout->addLayout(row);
    }

    return frame;
}


// ============================================================
// Refresh
// ============================================================

void TimelineWidget::refresh()
{
    refreshTimeline();
    refreshRightPanel();
    refreshCalendar();
}

void TimelineWidget::clearTimeline()
{
    if (!m_timelineLayout)
        return;

    while (m_timelineLayout->count() > 0)
    {
        QLayoutItem* item =
            m_timelineLayout->takeAt(0);

        if (!item)
            continue;

        if (QWidget* widget = item->widget())
            widget->deleteLater();

        delete item;
    }

    m_loadedStartDate = {};
    m_loadedEndDate = {};

    if (m_scrollArea)
    {
        m_scrollArea->verticalScrollBar()->setValue(0);
    }
}
// ============================================================
// Timeline refresh
// ============================================================

void TimelineWidget::refreshTimeline()
{
    if (!m_maintenanceManager)
        return;

    if (m_loadingTimeline)
        return;

    m_loadingTimeline = true;

    clearTimeline();

    m_topLoadZone = nullptr;
    m_bottomLoadZone = nullptr;

    if (!m_showEmptyDays)
    {
        m_topLoadZone =
            new QWidget();

        m_topLoadZone->setFixedHeight(180);

        m_topLoadZone->setAttribute(
            Qt::WA_TransparentForMouseEvents
            );

        m_timelineLayout->addWidget(
            m_topLoadZone
            );
    }

    const QDate today =
        QDate::currentDate();

    m_loadedStartDate =
        today.addDays(-20);

    m_loadedEndDate =
        today.addDays(20);

    for (
        QDate date = m_loadedStartDate;
        date <= m_loadedEndDate;
        date = date.addDays(1)
        )
    {
        addDay(date);
    }

    if (m_showEmptyDays)
    {
        m_timelineLayout->addStretch();
    }
    else
    {
        m_bottomLoadZone =
            new QWidget();

        m_bottomLoadZone->setFixedHeight(180);

        m_bottomLoadZone->setAttribute(
            Qt::WA_TransparentForMouseEvents
            );

        m_timelineLayout->addWidget(
            m_bottomLoadZone
            );

        m_topLoadZone->setFixedHeight(120);
        m_bottomLoadZone->setFixedHeight(120);

        m_topLoadZone->setStyleSheet(
            "background: transparent;"
            );

        m_bottomLoadZone->setStyleSheet(
            "background: transparent;"
            );
    }

    m_loadingTimeline = false;

    updateNextEventButton();
}


// ============================================================
// Event widget
// ============================================================

QWidget* TimelineWidget::createEventWidget(
    const MaintenanceEventData& event
    )
{
    auto* frame =
        new TimelineEventFrame(m_scrollArea);

    frame->setMinimumHeight(92);

    const bool selected =
        event.id == m_selectedEventId;

    if (selected)
    {
        frame->setStyleSheet(
            "QFrame {"
            "    background: #f8fbff;"
            "    border: 1px solid #3b82f6;"
            "    border-radius: 10px;"
            "}"
            );
    }
    else
    {
        frame->setStyleSheet(
            "QFrame {"
            "    background: #ffffff;"
            "    border: 1px solid #e2e8f0;"
            "    border-radius: 10px;"
            "}"
            "QFrame:hover {"
            "    border: 1px solid #93c5fd;"
            "}"
            );
    }
    /*
     * We explicitly use LTR for the physical order:
     *
     * icon | title/description | date/asset | status
     *
     * Individual labels remain RTL.
     */
    auto* layout =
        new QHBoxLayout(frame);

    layout->setDirection(
        QBoxLayout::LeftToRight
        );

    layout->setContentsMargins(
        14, 10, 14, 10
        );

    layout->setSpacing(14);

    // ========================================================
    // Status icon
    // ========================================================

    auto* iconContainer =
        new QFrame(frame);

    iconContainer->setFixedSize(
        48, 48
        );

    iconContainer->setStyleSheet(
        QString(
            "QFrame {"
            "    background: %1;"
            "    border: none;"
            "    border-radius: 24px;"
            "}"
            )
            .arg(
                statusBackgroundColor(
                    event.status
                    )
                )
        );

    auto* iconLabel =
        new QLabel(
            statusIcon(event.status),
            iconContainer
            );

    iconLabel->setAlignment(
        Qt::AlignCenter
        );

    iconLabel->setGeometry(
        iconContainer->rect()
        );

    iconLabel->setStyleSheet(
        QString(
            "font-size: 22px;"
            "font-weight: 700;"
            "color: %1;"
            "border: none;"
            "background: transparent;"
            )
            .arg(
                statusColor(event.status)
                )
        );

    layout->addWidget(
        iconContainer,
        0,
        Qt::AlignVCenter
        );

    // ========================================================
    // Title + description
    // ========================================================

    auto* titleDescriptionLayout =
        new QVBoxLayout();

    titleDescriptionLayout->setSpacing(
        5
        );

    titleDescriptionLayout->setContentsMargins(
        0, 15, 0, 0
        );
    auto* titleLabel =
        new QLabel(
            event.title,
            frame
            );

    titleLabel->setWordWrap(true);

    titleLabel->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #172554;"
        "border: none;"
        );

    auto* descriptionLabel =
        new QLabel(
            event.description,
            frame
            );

    descriptionLabel->setWordWrap(true);

    descriptionLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #64748b;"
        "border: none;"
        );

    titleDescriptionLayout->addWidget(
        titleLabel
        );

    if (!event.description.isEmpty())
    {
        titleDescriptionLayout->addWidget(
            descriptionLabel
            );
    }

    titleDescriptionLayout->addStretch();

    layout->addLayout(
        titleDescriptionLayout,
        1
        );

    // ========================================================
    // Separator
    // ========================================================

    auto* separator =
        new QFrame(frame);

    separator->setFrameShape(
        QFrame::VLine
        );

    separator->setFrameShadow(
        QFrame::Plain
        );

    separator->setStyleSheet(
        "color: #e2e8f0;"
        "background: #e2e8f0;"
        "border: none;"
        );

    layout->addWidget(
        separator
        );

    // ========================================================
    // Date + asset
    // ========================================================

    auto* metadataLayout =
        new QVBoxLayout();

    metadataLayout->setSpacing(
        6
        );

    metadataLayout->setContentsMargins(
        0, 0, 0, 0
        );

    auto* dateLabel =
        new QLabel(
            formatPersianDate(
                event.scheduledDate
                ),
            frame
            );

    dateLabel->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #64748b;"
        "border: none;"
        );

    QString assetText =
        event.assetName;

    if (!event.subAssetName.isEmpty())
    {
        assetText +=
            "  /  " +
            event.subAssetName;
    }

    auto* assetLabel =
        new QLabel(
            assetText,
            frame
            );

    assetLabel->setWordWrap(true);

    assetLabel->setStyleSheet(
        "font-size: 11px;"
        "color: #64748b;"
        "border: none;"
        );

    metadataLayout->addWidget(
        dateLabel
        );

    metadataLayout->addWidget(
        assetLabel
        );

    metadataLayout->addStretch();

    layout->addLayout(
        metadataLayout
        );

    // ========================================================
    // Status label
    // ========================================================

    auto* statusLabel =
        new QLabel(
            statusText(event.status),
            frame
            );

    statusLabel->setAlignment(
        Qt::AlignCenter
        );

    statusLabel->setMinimumWidth(
        82
        );

    statusLabel->setStyleSheet(
        QString(
            "background: %1;"
            "color: %2;"
            "border: none;"
            "border-radius: 14px;"
            "padding: 6px 10px;"
            "font-size: 11px;"
            "font-weight: 700;"
            )
            .arg(
                statusBackgroundColor(
                    event.status
                    )
                )
            .arg(
                statusColor(
                    event.status
                    )
                )
        );

    layout->addWidget(
        statusLabel,
        0,
        Qt::AlignVCenter
        );

    auto* eventFrame =
        static_cast<TimelineEventFrame*>(frame);

    eventFrame->onClicked =
        [this, event]()
    {
        selectEvent(event);
    };

    return frame;
}


// ============================================================
// Right panel refresh
// ============================================================

void TimelineWidget::refreshRightPanel()
{
    if (!m_maintenanceManager)
        return;

    int total = 0;
    int completed = 0;
    int overdue = 0;

    for (
        const MaintenanceEvent& event :
        m_maintenanceManager->events()
        )
    {
        const MaintenanceEventData& data =
            event.data();

        ++total;

        if (
            data.status ==
            MaintenanceStatus::Completed
            )
        {
            ++completed;
        }

        if (
            data.status ==
                MaintenanceStatus::Overdue ||
            data.status ==
                MaintenanceStatus::Missed
            )
        {
            ++overdue;
        }
    }

    if (m_totalEventsLabel)
        m_totalEventsLabel->setText(
            QString::number(total)
            );

    if (m_completedEventsLabel)
        m_completedEventsLabel->setText(
            QString::number(completed)
            );

    if (m_overdueEventsLabel)
        m_overdueEventsLabel->setText(
            QString::number(overdue)
            );

    updateSelectedEventInfo();
}


// ============================================================
// Events for date
// ============================================================

QList<MaintenanceEventData>
TimelineWidget::eventsForDate(
    const QDate& date
    ) const
{
    if (!m_maintenanceManager)
        return {};

    return m_maintenanceManager->eventsForDate(date);
}


// ============================================================
// Persian date
// ============================================================

QString TimelineWidget::formatPersianDate(
    const QDate& date
    ) const
{
    const QCalendar jalali(
        QCalendar::System::Jalali
        );

    const auto parts =
        jalali.partsFromDate(date);

    if (!parts.isValid())
        return {};

    return toPersianDigits(
        QString("%1/%2/%3")
            .arg(
                parts.year,
                4,
                10,
                QChar('0')
                )
            .arg(
                parts.month,
                2,
                10,
                QChar('0')
                )
            .arg(
                parts.day,
                2,
                10,
                QChar('0')
                )
        );
}


// ============================================================
// Day names
// ============================================================

QString TimelineWidget::dayName(
    const QDate& date
    ) const
{
    switch (date.dayOfWeek())
    {
    case Qt::Saturday:
        return "شنبه";

    case Qt::Sunday:
        return "یکشنبه";

    case Qt::Monday:
        return "دوشنبه";

    case Qt::Tuesday:
        return "سه‌شنبه";

    case Qt::Wednesday:
        return "چهارشنبه";

    case Qt::Thursday:
        return "پنجشنبه";

    case Qt::Friday:
        return "جمعه";
    }

    return {};
}


// ============================================================
// Status helpers
// ============================================================

QString TimelineWidget::statusText(
    MaintenanceStatus status
    ) const
{
    switch (status)
    {
    case MaintenanceStatus::Scheduled:
        return "برنامه‌ریزی شده";

    case MaintenanceStatus::InProgress:
        return "در حال انجام";

    case MaintenanceStatus::WaitingForHandling:
        return "در انتظار";

    case MaintenanceStatus::Completed:
        return "انجام شده";

    case MaintenanceStatus::Cancelled:
        return "لغو شده";

    case MaintenanceStatus::Overdue:
        return "تاخیر خورده";

    case MaintenanceStatus::Missed:
        return "انجام نشده";
    }

    return {};
}


QString TimelineWidget::statusIcon(
    MaintenanceStatus status
    ) const
{
    switch (status)
    {
    case MaintenanceStatus::Completed:
        return "✓";

    case MaintenanceStatus::Overdue:
        return "!";

    case MaintenanceStatus::Missed:
        return "×";

    case MaintenanceStatus::Scheduled:
        return "▣";

    case MaintenanceStatus::InProgress:
        return "●";

    case MaintenanceStatus::WaitingForHandling:
        return "…";

    case MaintenanceStatus::Cancelled:
        return "×";
    }

    return "?";
}


QString TimelineWidget::statusColor(
    MaintenanceStatus status
    ) const
{
    switch (status)
    {
    case MaintenanceStatus::Completed:
        return "#16a34a";

    case MaintenanceStatus::Overdue:
        return "#f59e0b";

    case MaintenanceStatus::Missed:
        return "#ef4444";

    case MaintenanceStatus::Scheduled:
        return "#2563eb";

    case MaintenanceStatus::InProgress:
        return "#7c3aed";

    case MaintenanceStatus::WaitingForHandling:
        return "#64748b";

    case MaintenanceStatus::Cancelled:
        return "#64748b";
    }

    return "#64748b";
}


QString TimelineWidget::statusBackgroundColor(
    MaintenanceStatus status
    ) const
{
    switch (status)
    {
    case MaintenanceStatus::Completed:
        return "#dcfce7";

    case MaintenanceStatus::Overdue:
        return "#fef3c7";

    case MaintenanceStatus::Missed:
        return "#fee2e2";

    case MaintenanceStatus::Scheduled:
        return "#dbeafe";

    case MaintenanceStatus::InProgress:
        return "#ede9fe";

    case MaintenanceStatus::WaitingForHandling:
        return "#f1f5f9";

    case MaintenanceStatus::Cancelled:
        return "#f1f5f9";
    }

    return "#f1f5f9";
}

void TimelineWidget::selectEvent(
    const MaintenanceEventData& event
    )
{
    m_selectedEventId =
        event.id;

    updateSelectedEventInfo();

    refreshTimeline();
}

void TimelineWidget::updateSelectedEventInfo()
{
    if (!m_assetManager)
        return;

    if (m_selectedEventId.isNull())
    {
        m_assetNameLabel->setText(
            "رویدادی انتخاب نشده"
            );

        m_assetTypeLabel->clear();
        m_assetSerialLabel->clear();
        m_assetImageLabel->clear();

        return;
    }

    MaintenanceEvent* event =
        m_maintenanceManager->getEvent(
            m_selectedEventId
            );

    if (!event)
        return;

    const MaintenanceEventData& eventData =
        event->data();

    Asset* asset =
        m_assetManager->getAsset(
            eventData.assetId
            );

    if (!asset)
    {
        m_assetNameLabel->setText(
            eventData.assetName
            );

        m_assetTypeLabel->clear();

        m_assetSerialLabel->clear();

        m_assetImageLabel->clear();

        return;
    }

    const AssetInfo& info =
        asset->info();

    m_assetNameLabel->setText(
        info.name
        );

    m_assetTypeLabel->setText(
        info.assetType
        );

    m_assetSerialLabel->setText(
        "سریال: " +
        info.serialNumber
        );

    if (!info.imagePath.isEmpty())
    {
        QPixmap pixmap(
            info.imagePath
            );

        if (!pixmap.isNull())
        {
            m_assetImageLabel->setPixmap(
                pixmap.scaled(
                    72,
                    72,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    )
                );
        }
        else
        {
            m_assetImageLabel->clear();
            m_assetImageLabel->setText("⚙");
        }
    }
    else
    {
        m_assetImageLabel->clear();
        m_assetImageLabel->setText("⚙");
    }
}

void TimelineWidget::clearSelectedEvent()
{
    m_selectedEventId =
        QUuid();

    updateSelectedEventInfo();

    refreshTimeline();
}

void TimelineWidget::refreshCalendar()
{
    if (!m_calendarWidget || !m_maintenanceManager)
        return;

    QList<CalendarEventDot> dots;

    // Look through the currently relevant calendar range.
    const QDate startDate =
        QDate::currentDate().addMonths(-1);

    const QDate endDate =
        QDate::currentDate().addMonths(12);

    for (QDate date = startDate;
         date <= endDate;
         date = date.addDays(1))
    {
        const QList<MaintenanceEventData> events =
            m_maintenanceManager->eventsForDate(date);

        for (const MaintenanceEventData& event : events)
        {
            CalendarEventDot dot;

            dot.date = date;

            dot.color =
                statusColor(event.status);

            dots.append(dot);
        }
    }

    m_calendarWidget->setEventDots(dots);
}
void TimelineWidget::addDay(
    const QDate& date,
    bool prepend
    )
{
    if (!date.isValid())
        return;

    const QList<MaintenanceEventData> events =
        eventsForDate(date);

    if (events.isEmpty() && !m_showEmptyDays)
        return;

    auto* dayRow =
        new QWidget();

    auto* dayLayout =
        new QHBoxLayout(dayRow);

    dayLayout->setContentsMargins(
        0, 8, 0, 4
        );

    dayLayout->setSpacing(10);

    auto* dayLabel =
        new QLabel(dayRow);

    QString dayText;

    if (date == QDate::currentDate())
    {
        dayText =
            QString("امروز  •  %1")
                .arg(formatPersianDate(date));
    }
    else
    {
        dayText =
            QString("%1  •  %2")
                .arg(dayName(date))
                .arg(formatPersianDate(date));
    }

    dayLabel->setText(dayText);

    dayLabel->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #94a3b8;"
        "border: none;"
        );

    dayRow->setProperty(
        "timelineDate",
        date
        );

    auto* separator =
        new QFrame(dayRow);

    separator->setFrameShape(
        QFrame::HLine
        );

    separator->setFrameShadow(
        QFrame::Plain
        );

    separator->setFixedHeight(1);

    separator->setStyleSheet(
        "background: #e2e8f0;"
        "border: none;"
        );

    dayLayout->addWidget(
        dayLabel,
        0
        );

    dayLayout->addWidget(
        separator,
        1,
        Qt::AlignVCenter
        );

    if (prepend)
    {
        const int index =
            m_topLoadZone
                ? 1
                : 0;

        m_timelineLayout->insertWidget(
            index,
            dayRow
            );
    }
    else
    {
        const int index =
            m_bottomLoadZone
                ? m_timelineLayout->indexOf(
                      m_bottomLoadZone
                      )
                : m_timelineLayout->count() - 1;

        m_timelineLayout->insertWidget(
            index,
            dayRow
            );
    }

    if (events.isEmpty())
    {
        auto* emptyLabel =
            createEmptyDayWidget(date);

        if (prepend)
        {
            const int index =
                m_topLoadZone
                    ? 2
                    : 1;

            m_timelineLayout->insertWidget(
                index,
                emptyLabel
                );
        }
        else
        {
            const int index =
                m_bottomLoadZone
                    ? m_timelineLayout->indexOf(
                          m_bottomLoadZone
                          )
                    : m_timelineLayout->count() - 1;

            m_timelineLayout->insertWidget(
                index,
                emptyLabel
                );
        }

        return;
    }

    for (const MaintenanceEventData& event : events)
    {
        auto* widget =
            createEventWidget(event);

        if (prepend)
        {
            const int index =
                m_topLoadZone
                    ? 2
                    : 1;

            m_timelineLayout->insertWidget(
                index,
                widget
                );
        }
        else
        {
            const int index =
                m_bottomLoadZone
                    ? m_timelineLayout->indexOf(
                          m_bottomLoadZone
                          )
                    : m_timelineLayout->count() - 1;

            m_timelineLayout->insertWidget(
                index,
                widget
                );
        }
    }
}

QWidget* TimelineWidget::createEmptyDayWidget(
    const QDate& date
    )
{
    Q_UNUSED(date);

    auto* label =
        new QLabel(
            "هیچ رویدادی برای این روز وجود ندارد."
            );

    label->setAlignment(
        Qt::AlignCenter
        );

    label->setMinimumHeight(42);

    label->setStyleSheet(
        "color: #94a3b8;"
        "font-size: 11px;"
        "background: #f8fafc;"
        "border: 1px solid #f1f5f9;"
        "border-radius: 8px;"
        "padding: 8px;"
        );

    return label;
}

void TimelineWidget::appendDays(
    int count
    )
{
    if (m_loadingTimeline ||
        count <= 0)
    {
        return;
    }

    m_loadingTimeline = true;

    for (int i = 1; i <= count; ++i)
    {
        const QDate date =
            m_loadedEndDate.addDays(i);

        addDay(date);
    }

    m_loadedEndDate =
        m_loadedEndDate.addDays(count);

    m_loadingTimeline = false;

    updateNextEventButton();
}

void TimelineWidget::prependDays(
    int count
    )
{
    if (m_loadingTimeline ||
        count <= 0)
    {
        return;
    }

    m_loadingTimeline = true;

    QScrollBar* bar =
        m_scrollArea->verticalScrollBar();

    const int oldValue =
        bar->value();

    const int oldMaximum =
        bar->maximum();

    for (int i = count; i >= 1; --i)
    {
        const QDate date =
            m_loadedStartDate.addDays(-i);

        addDay(
            date,
            true
            );
    }

    m_loadedStartDate =
        m_loadedStartDate.addDays(-count);

    QApplication::processEvents();

    const int newMaximum =
        bar->maximum();

    bar->setValue(
        oldValue +
        (newMaximum - oldMaximum)
        );

    m_loadingTimeline = false;

    updateNextEventButton();
}
void TimelineWidget::checkScrollPosition()
{
    if (m_loadingTimeline ||
        m_loadingMoreDays ||
        !m_scrollArea)
    {
        return;
    }

    QScrollBar* bar =
        m_scrollArea->verticalScrollBar();

    const int value =
        bar->value();

    const int maximum =
        bar->maximum();

    constexpr int threshold = 120;

    if (value <= threshold)
    {
        loadMoreDays(true);
        return;
    }

    if (value >= maximum - threshold)
    {
        loadMoreDays(false);
    }
}
QDate TimelineWidget::findNextEventDate() const
{
    if (!m_maintenanceManager)
        return {};

    return m_maintenanceManager
        ->nextScheduledEventDate();
}

void TimelineWidget::updateNextEventButton()
{
    if (!m_nextEventButton)
        return;

    bool hasEvent = false;

    for (
        QDate date = m_loadedStartDate;
        date <= m_loadedEndDate;
        date = date.addDays(1)
        )
    {
        if (!eventsForDate(date).isEmpty())
        {
            hasEvent = true;
            break;
        }
    }

    const QDate nextEvent =
        findNextEventDate();

    const bool shouldShow =
        !hasEvent &&
        nextEvent.isValid();

    m_nextEventButton->setVisible(
        shouldShow
        );

    if (shouldShow)
    {
        positionNextEventButton();
        m_nextEventButton->raise();
    }
}

void TimelineWidget::positionNextEventButton()
{
    if (!m_nextEventButton ||
        !m_scrollArea)
    {
        return;
    }

    QWidget* viewport =
        m_scrollArea->viewport();

    const int margin = 16;

    const QSize size =
        m_nextEventButton->sizeHint();

    m_nextEventButton->resize(size);

    m_nextEventButton->move(
        viewport->width()
            - size.width()
            - margin,

        viewport->height()
            - size.height()
            - margin
        );

    m_nextEventButton->raise();
}

void TimelineWidget::resizeEvent(
    QResizeEvent* event
    )
{
    QWidget::resizeEvent(event);

    positionNextEventButton();

    if (m_loadingIndicator &&
        m_loadingIndicator->isVisible())
    {
        // Keep the indicator attached to the
        // bottom of the viewport.
        showLoadingIndicator(false);
    }
}

void TimelineWidget::scrollToDate(
    const QDate& date
    )
{
    if (!m_scrollArea ||
        !m_timelineLayout)
    {
        return;
    }

    for (int i = 0;
         i < m_timelineLayout->count();
         ++i)
    {
        QLayoutItem* item =
            m_timelineLayout->itemAt(i);

        QWidget* widget =
            item ? item->widget() : nullptr;

        if (!widget)
            continue;

        const QVariant property =
            widget->property(
                "timelineDate"
                );

        if (!property.isValid())
            continue;

        if (
            property.toDate() != date
            )
        {
            continue;
        }

        QTimer::singleShot(
            0,
            this,
            [this, widget]()
            {
                m_scrollArea->ensureWidgetVisible(
                    widget,
                    0,
                    0
                    );
            }
            );

        return;
    }
}

bool TimelineWidget::eventFilter(
    QObject* watched,
    QEvent* event
    )
{
    if (watched == m_scrollArea->viewport() &&
        event->type() == QEvent::Resize)
    {
        positionNextEventButton();
    }

    return QWidget::eventFilter(
        watched,
        event
        );
}

void TimelineWidget::resetTimeline()
{
    if (!m_maintenanceManager ||
        m_loadingTimeline)
    {
        return;
    }

    const QDate today =
        QDate::currentDate();

    buildTimelineAroundDate(today);
}
void TimelineWidget::scrollToDateTop(
    const QDate& date
    )
{
    if (!m_timelineLayout ||
        !m_scrollArea)
    {
        return;
    }

    QWidget* target = nullptr;

    for (
        int i = 0;
        i < m_timelineLayout->count();
        ++i
        )
    {
        QLayoutItem* item =
            m_timelineLayout->itemAt(i);

        if (!item)
            continue;

        QWidget* widget =
            item->widget();

        if (!widget)
            continue;

        if (
            widget->property("timelineDate")
                .toDate() == date
            )
        {
            target = widget;
            break;
        }
    }

    if (!target)
        return;

    m_scrollArea->verticalScrollBar()->setValue(
        target->geometry().top()
        );
}

void TimelineWidget::showEvent(
    QShowEvent* event
    )
{
    QWidget::showEvent(event);

    resetTimeline();
}

void TimelineWidget::showLoadingIndicator(
    bool atTop
    )
{
    if (!m_loadingIndicator ||
        !m_scrollArea)
    {
        return;
    }

    QWidget* viewport =
        m_scrollArea->viewport();

    const int margin = 12;

    const int x =
        (viewport->width()
         - m_loadingIndicator->width()) / 2;

    const int y =
        atTop
            ? margin
            : viewport->height()
                  - m_loadingIndicator->height()
                  - margin;

    m_loadingIndicator->move(x, y);
    m_loadingIndicator->raise();
    m_loadingIndicator->show();

    m_loadingAnimationFrame = 0;

    if (m_loadingAnimationTimer)
        m_loadingAnimationTimer->start();
}

void TimelineWidget::hideLoadingIndicator()
{
    if (m_loadingAnimationTimer)
        m_loadingAnimationTimer->stop();

    if (m_loadingIndicator)
        m_loadingIndicator->hide();
}

void TimelineWidget::loadMoreDays(
    bool towardTop
    )
{
    if (m_loadingTimeline ||
        m_loadingMoreDays)
    {
        return;
    }

    m_loadingMoreDays = true;

    showLoadingIndicator(towardTop);

    QTimer::singleShot(
        350,
        this,
        [this, towardTop]()
        {
            if (towardTop)
            {
                prependDays(30);
            }
            else
            {
                appendDays(30);
            }

            m_loadingMoreDays = false;

            hideLoadingIndicator();
        }
        );
}

void TimelineWidget::showCalendarDate(
    const QDate& date
    )
{
    if (!date.isValid() ||
        !m_maintenanceManager)
    {
        return;
    }

    const QList<MaintenanceEventData> events =
        eventsForDate(date);

    // When empty days are hidden, clicking a day
    // without an event does absolutely nothing.
    if (events.isEmpty() && !m_showEmptyDays)
        return;

    buildTimelineAroundDate(date);
}

void TimelineWidget::buildTimelineAroundDate(
    const QDate& centerDate
    )
{
    if (!centerDate.isValid() ||
        !m_maintenanceManager ||
        m_loadingTimeline)
    {
        return;
    }

    m_loadingTimeline = true;

    clearTimeline();

    m_loadedStartDate =
        centerDate.addDays(-20);

    m_loadedEndDate =
        centerDate.addDays(20);

    // --------------------------------------------------------
    // Top load zone
    // --------------------------------------------------------

    if (!m_showEmptyDays)
    {
        m_topLoadZone =
            new QWidget();

        m_topLoadZone->setFixedHeight(120);

        m_topLoadZone->setAttribute(
            Qt::WA_TransparentForMouseEvents
            );

        m_topLoadZone->setStyleSheet(
            "background: transparent;"
            );

        m_timelineLayout->addWidget(
            m_topLoadZone
            );
    }

    // --------------------------------------------------------
    // Days
    // --------------------------------------------------------

    for (
        QDate date = m_loadedStartDate;
        date <= m_loadedEndDate;
        date = date.addDays(1)
        )
    {
        addDay(date);
    }

    // --------------------------------------------------------
    // Bottom load zone
    // --------------------------------------------------------

    if (!m_showEmptyDays)
    {
        m_bottomLoadZone =
            new QWidget();

        m_bottomLoadZone->setFixedHeight(120);

        m_bottomLoadZone->setAttribute(
            Qt::WA_TransparentForMouseEvents
            );

        m_bottomLoadZone->setStyleSheet(
            "background: transparent;"
            );

        m_timelineLayout->addWidget(
            m_bottomLoadZone
            );
    }
    else
    {
        m_timelineLayout->addStretch();
    }

    m_loadingTimeline = false;

    updateNextEventButton();

    // Put the selected date at the top.
    QTimer::singleShot(
        0,
        this,
        [this, centerDate]()
        {
            scrollToDateTop(centerDate);
        }
        );
}