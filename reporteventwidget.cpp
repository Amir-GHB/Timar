#include "reporteventwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QIcon>

#include "persiandigits.h"


ReportEventWidget::ReportEventWidget(
    const MaintenanceEventData& eventData,
    QWidget* parent
    )
    : QWidget(parent)
    , m_eventData(eventData)
{

    setupUI();
    updateUI();
}


// ============================================================
// UI
// ============================================================

void ReportEventWidget::setupUI()
{
    setObjectName("reportEventWidget");
    setAttribute(Qt::WA_StyledBackground, true);
    setLayoutDirection(Qt::RightToLeft);

    setStyleSheet(
        "#reportEventWidget {"
        "    background-color: #ffffff;"
        "    border: 1px solid #dce4f2;"
        "    border-radius: 12px;"
        "}"
        );

    setMinimumWidth(650);
    setFixedHeight(100);

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(18, 14, 18, 12);
    mainLayout->setSpacing(10);


    // ========================================================
    // HEADER
    // ========================================================

    auto* headerWidget = new QWidget(this);

    auto* headerLayout = new QHBoxLayout(headerWidget);

    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(12);


    // --------------------------------------------------------
    // Event icon
    // --------------------------------------------------------

    auto* iconLabel = new QLabel(headerWidget);

    iconLabel->setFixedSize(42, 42);
    iconLabel->setAlignment(Qt::AlignCenter);

    iconLabel->setPixmap(
        QIcon(
            ":/qlementine/icons/custome/one-gear-blue.svg"
            ).pixmap(25, 25)
        );

    iconLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #e9efff;"
        "    border-radius: 12px;"
        "}"
        );


    // --------------------------------------------------------
    // Title + asset
    // --------------------------------------------------------

    auto* titleWidget = new QWidget(headerWidget);

    auto* titleLayout = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(3);

    m_titleLabel = new QLabel(titleWidget);

    m_titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 15px;"
        "    font-weight: 600;"
        "    color: #172554;"
        "}"
        );

    m_typeLabel = new QLabel(titleWidget);

    m_typeLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 11px;"
        "    color: #64748b;"
        "}"
        );

    titleLayout->addWidget(m_titleLabel);
    titleLayout->addWidget(m_typeLabel);

    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(titleWidget, 1);

    // --------------------------------------------------------
    // Date + installation place
    // --------------------------------------------------------

    auto* infoWidget = new QWidget(headerWidget);

    auto* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(5);

    auto* dateRow = new QHBoxLayout();
    dateRow->setContentsMargins(0, 0, 0, 0);
    dateRow->setSpacing(6);

    // Date icon
    auto* dateIcon = new QLabel(infoWidget);
    dateIcon->setFixedSize(16, 16);
    dateIcon->setAlignment(Qt::AlignCenter);
    dateIcon->setPixmap(
        QIcon(":/qlementine/icons/custome/calendar.svg")
            .pixmap(14, 14)
        );

    // Date title
    auto* dateTitle = new QLabel("تاریخ اجرا:", infoWidget);
    dateTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 11px;"
        "    color: #718096;"
        "}"
        );

    m_dateLabel = new QLabel(infoWidget);
    m_dateLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 11px;"
        "    font-weight: 600;"
        "    color: #172554;"
        "}"
        );

    dateRow->addWidget(dateIcon);
    dateRow->addWidget(dateTitle);
    dateRow->addWidget(m_dateLabel);
    dateRow->addStretch();

    auto* placeRow = new QHBoxLayout();
    placeRow->setContentsMargins(0, 0, 0, 0);
    placeRow->setSpacing(6);

    // Installation place icon
    auto* placeIcon = new QLabel(infoWidget);
    placeIcon->setFixedSize(16, 16);
    placeIcon->setAlignment(Qt::AlignCenter);
    placeIcon->setPixmap(
        QIcon(":/qlementine/icons/custome/location.svg")
            .pixmap(14, 14)
        );

    // Installation place title
    auto* placeTitle = new QLabel("محل نصب:", infoWidget);
    placeTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 11px;"
        "    color: #718096;"
        "}"
        );

    m_placeLabel = new QLabel(infoWidget);
    m_placeLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 11px;"
        "    font-weight: 600;"
        "    color: #172554;"
        "}"
        );

    placeRow->addWidget(placeIcon);
    placeRow->addWidget(placeTitle);
    placeRow->addWidget(m_placeLabel);
    placeRow->addStretch();

    infoLayout->addLayout(dateRow);
    infoLayout->addLayout(placeRow);

    headerLayout->addWidget(infoWidget, 1);

    // --------------------------------------------------------
    // Status + details
    // --------------------------------------------------------

    auto* statusWidget = new QWidget(headerWidget);

    auto* statusLayout = new QVBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setSpacing(5);

    m_statusLabel = new QLabel(statusWidget);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setMinimumWidth(90);
    m_statusLabel->setFixedHeight(32);

    statusLayout->addWidget(
        m_statusLabel, 0, Qt::AlignRight
        );

    headerLayout->addWidget(statusWidget);

    //Showdetails Button

    auto* detailsWidget = new QWidget(headerWidget);

    auto* detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setContentsMargins(0, 0, 0, 0);
    detailsLayout->setSpacing(5);

    m_detailsButton = new QPushButton(
        "نمایش جزئیات",
        detailsWidget
        );

    m_detailsButton->setFocusPolicy(Qt::NoFocus);

    m_detailsButton->setIcon(
        QIcon(":/qlementine/icons/16/action/eye.svg")
        );

    m_detailsButton->setFixedSize(95, 32);

    m_detailsButton->setStyleSheet(
        "QPushButton {"
        "background-color: #eff6ff;"
        "color: #2563eb;"
        "border: 1px solid #bfdbfe;"
        "border-radius: 6px;"
        "font-size: 10px;"
        "padding: 0px 6px;"
        "}"
        "QPushButton:hover {"
        "background-color: #dbeafe;"
        "}"
        );

    detailsLayout->addWidget(
        m_detailsButton, 0, Qt::AlignRight
        );

    headerLayout->addWidget(detailsWidget, 1, Qt::AlignRight);
    mainLayout->addWidget(headerWidget);
    // ========================================================
    // CONNECTIONS
    // ========================================================

    connect(
        m_detailsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            emit detailsRequested(
                m_eventData.id
                );
        }
        );
}


// ============================================================
// Info item
// ============================================================

QWidget* ReportEventWidget::createInfoItem(
    const QString& iconPath,
    const QString& title,
    QLabel*& valueLabel
    )
{
    auto* widget = new QWidget(this);

    auto* mainLayout = new QVBoxLayout(widget);

    mainLayout->setContentsMargins(12, 0, 12, 0);
    mainLayout->setSpacing(2);


    // ========================================================
    // Title row
    // ========================================================

    auto* titleRow = new QWidget(widget);

    auto* titleLayout = new QHBoxLayout(titleRow);

    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(7);

    auto* icon = new QLabel(titleRow);

    icon->setFixedSize(18, 18);
    icon->setAlignment(Qt::AlignCenter);

    icon->setPixmap(
        QIcon(iconPath).pixmap(16, 16)
        );

    auto* titleLabel = new QLabel(
        title,
        titleRow
        );

    titleLabel->setAlignment(
        Qt::AlignRight | Qt::AlignVCenter
        );

    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 10px;"
        "    color: #718096;"
        "}"
        );

    titleLayout->addWidget(icon);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();


    // ========================================================
    // Value row
    // ========================================================

    auto* valueRow = new QWidget(widget);

    auto* valueLayout = new QHBoxLayout(valueRow);

    valueLayout->setContentsMargins(25, 0, 0, 0);
    valueLayout->setSpacing(0);

    valueLabel = new QLabel(
        "-",
        valueRow
        );

    valueLabel->setAlignment(
        Qt::AlignRight | Qt::AlignVCenter
        );

    valueLabel->setTextInteractionFlags(
        Qt::TextSelectableByMouse
        );

    valueLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "    color: #172554;"
        "}"
        );

    valueLayout->addWidget(valueLabel);


    // ========================================================
    // Final layout
    // ========================================================

    mainLayout->addWidget(titleRow);
    mainLayout->addWidget(valueRow);

    return widget;
}


// ============================================================
// Update UI
// ============================================================

void ReportEventWidget::updateUI()
{
    m_titleLabel->setText(
        m_eventData.title
        );

    m_typeLabel->setText(
        ""
        );

    QString assetText = m_eventData.assetName;

    if (!m_eventData.subAssetName.isEmpty())
    {
        assetText += " / " + m_eventData.subAssetName;
    }

    m_typeLabel->setText(assetText);

    m_dateLabel->setText(
        toPersianDate(m_eventData.scheduledDate)
        );

    m_placeLabel->setText(
        m_eventData.installationPlace.isEmpty()
            ? "-"
            : m_eventData.installationPlace
        );

    updateStatus();
}


// ============================================================
// Status
// ============================================================

void ReportEventWidget::updateStatus()
{
    QString text;
    QString background;
    QString foreground;

    switch (m_eventData.status)
    {
    case MaintenanceStatus::Scheduled:
        text = "برنامه‌ریزی شده";
        background = "#fff4d8";
        foreground = "#a16207";
        break;

    case MaintenanceStatus::WaitingForHandling:
        text = "در انتظار رسیدگی";
        background = "#eee7ff";
        foreground = "#6d4cc2";
        break;

    case MaintenanceStatus::InProgress:
        text = "در حال انجام";
        background = "#e2efff";
        foreground = "#2563eb";
        break;

    case MaintenanceStatus::Completed:
        text = "تکمیل شده";
        background = "#e1f7e7";
        foreground = "#15803d";
        break;

    case MaintenanceStatus::Cancelled:
        text = "لغو شده";
        background = "#eeeeee";
        foreground = "#6b7280";
        break;

    case MaintenanceStatus::Overdue:
        text = "معوق";
        background = "#fff0d5";
        foreground = "#c2410c";
        break;

    case MaintenanceStatus::Missed:
        text = "انجام نشده";
        background = "#ffe3e6";
        foreground = "#dc2626";
        break;
    }

    m_statusLabel->setText(text);

    m_statusLabel->setStyleSheet(
        QString(
            "QLabel {"
            "    background-color: %1;"
            "    color: %2;"
            "    border-radius: 16px;"
            "    padding: 0 12px;"
            "    font-size: 11px;"
            "    font-weight: 600;"
            "}"
            ).arg(background, foreground)
        );
}


// ============================================================
// Separator
// ============================================================

QFrame* ReportEventWidget::createInfoSeparator(
    QWidget* parent
    )
{
    auto* separator = new QFrame(parent);

    separator->setFixedWidth(1);
    separator->setFixedHeight(28);

    separator->setStyleSheet(
        "QFrame {"
        "    background-color: #e5eaf2;"
        "    border: none;"
        "}"
        );

    return separator;
}