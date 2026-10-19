#include "reportssection.h"

#include "maintenancemanager.h"
#include "maintenancereporttypes.h"
#include "persiandigits.h"
#include "reportcardwidget.h"
#include "reporteventwidget.h"
#include "assetmanager.h"

#include <QComboBox>
#include <QDateEdit>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPdfWriter>
#include <QPainter>
#include <QPageSize>
#include <QFontDatabase>
#include <qapplication.h>
#include <QCalendar>
#include <qdebug.h>
#include <qmenu.h>

ReportsSection::ReportsSection(
    MaintenanceManager& maintenanceManager,
    QWidget* parent
    )
    : QWidget(parent)
    , m_maintenanceManager(maintenanceManager)
{
    qDebug() << "reportsection: contruction";
    setObjectName("reportsWidget");
    setLayoutDirection(Qt::RightToLeft);
    qDebug() << " stage 1";
    auto* mainLayout = new QHBoxLayout(this);
    qDebug() << " stage 2";
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);
    qDebug() << " stage 3";
    mainLayout->addWidget(createResultsPanel());
    qDebug() << " stage 4";
    mainLayout->addWidget(createSidePanel(), 1);
    qDebug() << " trying populate";
    // Show the current report immediately.
    populateReport();

    connect(
        m_generateButton,
        &QPushButton::clicked,
        this,
        &ReportsSection::populateReport
        );

    connect(
        m_exportButton,
        &QPushButton::clicked,
        this,
        &ReportsSection::exportReport
        );
}

QWidget* ReportsSection::createResultsPanel()
{
    auto* panel = new QFrame(this);
    panel->setObjectName("reportsResultsPanel");

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    // ========================================================
    // TITLE
    // ========================================================

    auto* title = new QLabel("آمار کلی", panel);
    title->setObjectName("sectionTitle");

    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);


    // ========================================================
    // STATUS SUMMARY
    // ========================================================

    auto* summaryFrame = new QFrame(panel);
    summaryFrame->setObjectName("reportSummary");
    summaryFrame->setFixedHeight(100);

    summaryFrame->setStyleSheet(
        "QFrame#reportSummary {"
        "    background-color: white;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 12px;"
        "}"
        );

    auto* summaryLayout = new QHBoxLayout(summaryFrame);
    summaryLayout->setContentsMargins(10, 8, 10, 8);
    summaryLayout->setSpacing(8);


    // --------------------------------------------------------
    // Helper for status cards
    // --------------------------------------------------------

    auto createStatusCard =
        [summaryFrame, summaryLayout](
            const QString& titleText,
            const QString& iconPath,
            const QString& backgroundColor,
            const QString& textColor,
            QLabel*& countLabel)
    {
        auto* card = new QFrame(summaryFrame);

        card->setStyleSheet(
            QString(
                "QFrame {"
                "    background-color: %1;"
                "    border: none;"
                "    border-radius: 8px;"
                "}"
                ).arg(backgroundColor)
            );

        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(10, 6, 10, 6);
        cardLayout->setSpacing(3);


        // ----------------------------------------------------
        // Title + icon
        // ----------------------------------------------------

        auto* titleRow = new QHBoxLayout();
        titleRow->setContentsMargins(0, 0, 0, 0);
        titleRow->setSpacing(5);

        auto* icon = new QLabel(card);
        icon->setFixedSize(25, 25);
        icon->setAlignment(Qt::AlignCenter);

        icon->setPixmap(
            QIcon(iconPath).pixmap(24, 24)
            );

        auto* titleLabel =
            new QLabel(titleText, card);

        titleRow->addWidget(icon);
        titleRow->addWidget(titleLabel);
        titleRow->addStretch();

        // --------------------------------------------------------
        // Count
        // --------------------------------------------------------

        countLabel = new QLabel("0", card);

        countLabel->setStyleSheet(
            "QLabel {"
            "    color: #0f172a;"
            "    font-size: 20px;"
            "    font-weight: 600;"
            "    background: transparent;"
            "}"
            );

        auto* countRow = new QHBoxLayout();
        countRow->setContentsMargins(21, 0, 0, 0);
        countRow->setSpacing(0);

        countRow->addWidget(countLabel);
        countRow->addStretch();
        countRow->addStretch();


        cardLayout->addLayout(titleRow);
        cardLayout->addLayout(countRow);

        summaryLayout->addWidget(card, 1);
    };


    // --------------------------------------------------------
    // Status cards
    // --------------------------------------------------------

    createStatusCard(
        "برنامه‌ریزی شده",
        ":/qlementine/icons/custome/status-schedueled.svg",
        "#eff6ff",
        "#2563eb",
        m_scheduledCount
        );

    createStatusCard(
        "تکمیل شده",
        ":/qlementine/icons/custome/status-comp.svg",
        "#ecfdf5",
        "#059669",
        m_completedCount
        );

    createStatusCard(
        "لغو شده",
        ":/qlementine/icons/custome/status-cancell.svg",
        "#f1f5f9",
        "#475569",
        m_cancelledCount
        );

    createStatusCard(
        "معوق",
        ":/qlementine/icons/custome/status-over.svg",
        "#fefce8",
        "#ca8a04",
        m_overdueCount
        );

    createStatusCard(
        "انجام نشده",
        ":/qlementine/icons/custome/status-miss.svg",
        "#fef2f2",
        "#dc2626",
        m_missedCount
        );

    layout->addWidget(summaryFrame);

    // ========================================================
    // EVENTS TITLE
    // ========================================================

    auto* eventsTitleWidget = new QWidget(panel);

    auto* eventsTitleLayout = new QHBoxLayout(eventsTitleWidget);
    eventsTitleLayout->setContentsMargins(0, 4, 0, 0);
    eventsTitleLayout->setSpacing(7);

    auto* eventsIcon = new QLabel(eventsTitleWidget);
    eventsIcon->setFixedSize(24, 24);
    eventsIcon->setAlignment(Qt::AlignCenter);

    eventsIcon->setPixmap(
        QIcon(":/qlementine/icons/custome/event.svg").pixmap(22, 22)
        );

    auto* eventsTitle = new QLabel(
        "گزارش رویدادها",
        eventsTitleWidget
        );

    QFont eventsTitleFont = eventsTitle->font();
    eventsTitleFont.setPointSize(14);
    eventsTitleFont.setBold(true);
    eventsTitle->setFont(eventsTitleFont);

    eventsTitleLayout->addWidget(eventsIcon);
    eventsTitleLayout->addWidget(eventsTitle);
    eventsTitleLayout->addStretch();

    layout->addWidget(eventsTitleWidget);
    // ========================================================
    // RESULTS SCROLL AREA
    // ========================================================

    m_resultsScrollArea = new QScrollArea(panel);

    m_resultsScrollArea->setWidgetResizable(true);
    m_resultsScrollArea->setFrameShape(QFrame::NoFrame);
    m_resultsScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );


    m_resultsContainer = new QWidget();

    m_resultsContainer->setObjectName(
        "reportsResultsContainer"
        );


    m_resultsLayout =
        new QVBoxLayout(m_resultsContainer);

    m_resultsLayout->setContentsMargins(
        0, 0, 0, 0
        );

    m_resultsLayout->setSpacing(10);


    // Prevent cards from stretching vertically.
    m_resultsLayout->addStretch();


    m_resultsScrollArea->setWidget(
        m_resultsContainer
        );

    layout->addWidget(
        m_resultsScrollArea,
        1
        );


    return panel;
}

QWidget* ReportsSection::createSidePanel()
{

    auto* panel = new QFrame(this);
    panel->setObjectName("reportsSidePanel");
    panel->setFixedWidth(300);

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(14);

    // -------------------------
    // Event count graph
    // -------------------------
/*auto* graphFrame = new QFrame(panel);
    graphFrame->setObjectName("reportGraph");
    graphFrame->setStyleSheet(
        "QFrame#reportGraph {"
        "background-color: white;"
        "border: 1px solid #e2e8f0;"
        "border-radius: 12px;"
        "}"
        );

    auto* graphLayout = new QVBoxLayout(graphFrame);
    graphLayout->setContentsMargins(14, 14, 14, 14);
    graphLayout->setSpacing(10);

    auto* graphTitle = new QLabel("تعداد رویدادها", graphFrame);
    graphTitle->setObjectName("cardTitle");

    auto* graphPlaceholder =
        new QLabel("نمودار گزارش", graphFrame);

    graphPlaceholder->setAlignment(Qt::AlignCenter);
    graphPlaceholder->setMinimumHeight(150);

    graphLayout->addWidget(graphTitle);
    graphLayout->addWidget(graphPlaceholder);

    layout->addWidget(graphFrame);*/
    // -------------------------
    // Filters
    // -------------------------

    auto* filterWidget = new ReportCardWidget(panel);

    auto* filterLayout = new QVBoxLayout(filterWidget);
    filterLayout->setContentsMargins(14, 14, 14, 14);
    filterLayout->setSpacing(10);

    auto* filterTitle = new QLabel("فیلتر گزارش", filterWidget);
    filterTitle->setObjectName("cardTitle");
    filterLayout->addWidget(filterTitle);

    m_statusCombo = new QComboBox(filterWidget);
    m_statusCombo->addItem("همه وضعیت‌ها");
    m_statusCombo->addItem("برنامه‌ریزی شده");
    m_statusCombo->addItem("تکمیل شده");
    m_statusCombo->addItem("لغو شده");
    m_statusCombo->addItem("منقضی شده");
    m_statusCombo->addItem("انجام نشده");
    filterLayout->addWidget(m_statusCombo);
//Asset filteration menue
    // Asset filter
    auto* assetFilterLabel = new QLabel(
        "فیلتر تجهیزات",
        filterWidget
        );

    filterLayout->addWidget(assetFilterLabel);
    m_allAssetsCheckBox = new QCheckBox(
        "همه تجهیزات",
        filterWidget
        );
    m_allAssetsCheckBox->setTristate(true);
    filterLayout->addWidget(m_allAssetsCheckBox);
    m_assetScrollArea = new QScrollArea(filterWidget);
    m_assetScrollArea->setWidgetResizable(true);
    m_assetScrollArea->setFrameShape(QFrame::NoFrame);
    m_assetScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );
    m_assetScrollArea->setMinimumHeight(120);
    m_assetScrollArea->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );
    m_assetListWidget = new QWidget(m_assetScrollArea);
    m_assetListLayout = new QVBoxLayout(m_assetListWidget);
    m_assetListLayout->setContentsMargins(4, 4, 4, 4);
    m_assetListLayout->setSpacing(6);
    m_assetListLayout->setAlignment(Qt::AlignTop);
    m_assetScrollArea->setWidget(m_assetListWidget);
    filterLayout->addWidget(m_assetScrollArea, 1);
    populateAssetFilter();


    m_fromDateEdit = new PersianDateEdit(filterWidget);
    m_fromDateEdit->setDate(QDate::currentDate().addMonths(-1));
    filterLayout->addWidget(m_fromDateEdit);

    m_toDateEdit = new PersianDateEdit(filterWidget);
    m_toDateEdit->setDate(QDate::currentDate());
    filterLayout->addWidget(m_toDateEdit);

    // Date range preset buttons
    auto* rangeLayout = new QHBoxLayout();
    rangeLayout->setContentsMargins(0, 0, 0, 0);
    rangeLayout->setSpacing(6);

    auto* leftRangeLayout = new QVBoxLayout();
    leftRangeLayout->setContentsMargins(0, 0, 0, 0);
    leftRangeLayout->setSpacing(6);

    auto* rightRangeLayout = new QVBoxLayout();
    rightRangeLayout->setContentsMargins(0, 0, 0, 0);
    rightRangeLayout->setSpacing(6);

    auto* weekButton = new QPushButton("هفته جاری", filterWidget);
    auto* monthButton = new QPushButton("ماه جاری", filterWidget);
    auto* sixMonthsButton = new QPushButton("۶ ماه اخیر", filterWidget);
    auto* yearButton = new QPushButton("سال جاری", filterWidget);

    const QList<QPushButton*> rangeButtons = {
        weekButton, monthButton, sixMonthsButton, yearButton
    };

    for (auto* button : rangeButtons)
    {
        button->setMinimumHeight(32);
        button->setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Fixed
            );
        button->setFocusPolicy(Qt::NoFocus);
    }

    leftRangeLayout->addWidget(weekButton);
    leftRangeLayout->addWidget(monthButton);
    rightRangeLayout->addWidget(sixMonthsButton);
    rightRangeLayout->addWidget(yearButton);

    rangeLayout->addLayout(leftRangeLayout);
    rangeLayout->addLayout(rightRangeLayout);
    filterLayout->addLayout(rangeLayout);

    m_generateButton = new QPushButton("نمایش گزارش", filterWidget);
    filterLayout->addWidget(m_generateButton);

    connect(weekButton, &QPushButton::clicked,
            this, &ReportsSection::setCurrentWeekRange);
    connect(monthButton, &QPushButton::clicked,
            this, &ReportsSection::setCurrentMonthRange);
    connect(sixMonthsButton, &QPushButton::clicked,
            this, &ReportsSection::setLastSixMonthsRange);
    connect(yearButton, &QPushButton::clicked,
            this, &ReportsSection::setCurrentYearRange);

    layout->addWidget(filterWidget);
    // -------------------------
    // Export
    // -------------------------

    auto* exportWidget = new ReportCardWidget(panel);
    auto* exportLayout = new QVBoxLayout(exportWidget);
    exportLayout->setContentsMargins(14, 14, 14, 14);
    exportLayout->setSpacing(10);

    m_exportFormatCombo = new QComboBox(exportWidget);
    m_exportFormatCombo->addItem("CSV");
    m_exportFormatCombo->addItem("PDF");
    exportLayout->addWidget(m_exportFormatCombo);

    m_exportButton = new QPushButton("دریافت خروجی", exportWidget);
    exportLayout->addWidget(m_exportButton);

    layout->addWidget(exportWidget);
    layout->addStretch();
qDebug() << "Side panel returning";
    return panel;
}

void ReportsSection::clearResults()
{
    while (m_resultsLayout->count() > 1)
    {
        QLayoutItem* item = m_resultsLayout->takeAt(0);

        if (!item)
            continue;

        if (QWidget* widget = item->widget())
        {
            widget->deleteLater();
        }

        delete item;
    }
}

MaintenanceEventFilter ReportsSection::buildFilter() const
{
    MaintenanceEventFilter filter;

    filter.fromDate = m_fromDateEdit->date();
    filter.toDate = m_toDateEdit->date();

    switch (m_statusCombo->currentIndex())
    {
    case 1:
        filter.statuses.append(MaintenanceStatus::Scheduled);
        break;

    case 2:
        filter.statuses.append(MaintenanceStatus::Completed);
        break;

    case 3:
        filter.statuses.append(MaintenanceStatus::Cancelled);
        break;

    case 4:
        filter.statuses.append(MaintenanceStatus::Overdue);
        break;

    case 5:
        filter.statuses.append(MaintenanceStatus::Missed);
        break;

    default:
        break;
    }

    filter.assetFilterEnabled = true;

    for (QCheckBox* checkBox : m_assetChecks)
    {
        if (!checkBox->isChecked())
            continue;

        const QUuid assetId =
            checkBox->property("assetId").toUuid();

        if (!assetId.isNull())
            filter.assetIds.append(assetId);
    }

    return filter;
}

void ReportsSection::populateReport()
{
    qDebug() << "reportsection: populate report";
    clearResults();

    const MaintenanceEventFilter filter = buildFilter();

    const QList<MaintenanceEventData> events =
        m_maintenanceManager.eventsForFilter(filter);

    for (const MaintenanceEventData& eventData : events)
    {
        auto* eventWidget =
            new ReportEventWidget(
                eventData,
                m_resultsContainer
                );

        m_resultsLayout->insertWidget(
            m_resultsLayout->count() - 1,
            eventWidget
            );

        connect(
            eventWidget,
            &ReportEventWidget::detailsRequested,
            this,
            [this](const QUuid& eventId)
            {
                // Details dialog/page will be added here later.
                Q_UNUSED(eventId);
            }
            );
    }
    updateSummary();
}

void ReportsSection::updateSummary()
{
    MaintenanceEventFilter filter = buildFilter();

    // Summary always shows every status.
    filter.statuses.clear();

    const auto events =
        m_maintenanceManager.eventsForFilter(filter);

    int scheduled = 0;
    int completed = 0;
    int cancelled = 0;
    int overdue = 0;
    int missed = 0;

    for (const auto& event : events)
    {
        switch (event.status)
        {
        case MaintenanceStatus::Scheduled:
        case MaintenanceStatus::WaitingForHandling:
        case MaintenanceStatus::InProgress:
            ++scheduled;
            break;

        case MaintenanceStatus::Completed:
            ++completed;
            break;

        case MaintenanceStatus::Cancelled:
            ++cancelled;
            break;

        case MaintenanceStatus::Overdue:
            ++overdue;
            break;

        case MaintenanceStatus::Missed:
            ++missed;
            break;
        }
    }

    m_scheduledCount->setText(QString::number(scheduled));
    m_completedCount->setText(QString::number(completed));
    m_cancelledCount->setText(QString::number(cancelled));
    m_overdueCount->setText(QString::number(overdue));
    m_missedCount->setText(QString::number(missed));
}

void ReportsSection::exportReport()
{
    const MaintenanceEventFilter filter = buildFilter();

    const QList<MaintenanceEventData> events =
        m_maintenanceManager.eventsForFilter(filter);

    const QString format =
        m_exportFormatCombo->currentText();

    if (format == "CSV")
    {
        const QString filePath =
            QFileDialog::getSaveFileName(
                this,
                "ذخیره گزارش CSV",
                "گزارش نگهداری.csv",
                "CSV Files (*.csv)"
                );

        if (filePath.isEmpty())
            return;

        if (!exportCsv(filePath, events))
        {
            QMessageBox::critical(
                this,
                "خطا",
                "ذخیره فایل CSV انجام نشد."
                );

            return;
        }
    }
    else if (format == "PDF")
    {
        const QString filePath =
            QFileDialog::getSaveFileName(
                this,
                "ذخیره گزارش PDF",
                "گزارش نگهداری.pdf",
                "PDF Files (*.pdf)"
                );

        if (filePath.isEmpty())
            return;

        if (!exportPdf(filePath, events))
        {
            QMessageBox::critical(
                this,
                "خطا",
                "ذخیره فایل PDF انجام نشد."
                );

            return;
        }
    }
}

bool ReportsSection::exportCsv(
    const QString& filePath,
    const QList<MaintenanceEventData>& events
    )
{
    QFile file(filePath);

    if (!file.open(
            QIODevice::WriteOnly |
            QIODevice::Text
            ))
    {
        return false;
    }

    QTextStream stream(&file);

    // UTF-8
    stream.setCodec("UTF-8");

    // BOM helps Excel recognize Persian UTF-8 correctly.
    stream << QChar(0xFEFF);

    stream
        << "عنوان,"
        << "تجهیز,"
        << "زیر تجهیز,"
        << "محل نصب,"
        << "تاریخ اجرا,"
        << "وضعیت\n";

    auto escapeCsv =
        [](const QString& value)
    {
        QString result = value;

        result.replace("\"", "\"\"");

        if (result.contains(',')
            || result.contains('"')
            || result.contains('\n'))
        {
            result = "\"" + result + "\"";
        }

        return result;
    };

    auto statusText =
        [](MaintenanceStatus status)
    {
        switch (status)
        {
        case MaintenanceStatus::Scheduled:
            return QString("برنامه‌ریزی شده");

        case MaintenanceStatus::WaitingForHandling:
            return QString("در انتظار رسیدگی");

        case MaintenanceStatus::InProgress:
            return QString("در حال انجام");

        case MaintenanceStatus::Completed:
            return QString("تکمیل شده");

        case MaintenanceStatus::Cancelled:
            return QString("لغو شده");

        case MaintenanceStatus::Overdue:
            return QString("معوق");

        case MaintenanceStatus::Missed:
            return QString("انجام نشده");
        }

        return QString("نامشخص");
    };

    for (const MaintenanceEventData& event : events)
    {
        QString subAsset =
            event.subAssetName;

        QString date =
            toPersianDate(event.scheduledDate);

        stream
            << escapeCsv(event.title) << ','
            << escapeCsv(event.assetName) << ','
            << escapeCsv(subAsset) << ','
            << escapeCsv(event.installationPlace) << ','
            << escapeCsv(date) << ','
            << escapeCsv(statusText(event.status))
            << '\n';
    }

    file.close();

    return true;
}

bool ReportsSection::exportPdf(
    const QString& filePath,
    const QList<MaintenanceEventData>& events)
{
    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);
    writer.setResolution(96);

    QPainter painter(&writer);

    if (!painter.isActive())
        return false;

    const QRect pageRect = painter.viewport();

    const int margin = 45;
    const int left = margin;
    const int right = pageRect.width() - margin;
    const int top = margin;
    const int bottom = pageRect.height() - margin;

    const int tableWidth = right - left;

    // ---------------------------------------------------------
    // Fonts
    // ---------------------------------------------------------

    QFont baseFont = QApplication::font();
    baseFont.setFamily("Vazirmatn");
    baseFont.setPointSize(9);

    QFont titleFont = baseFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);

    QFont subtitleFont = baseFont;
    subtitleFont.setPointSize(9);

    QFont headerFont = baseFont;
    headerFont.setPointSize(9);
    headerFont.setBold(true);

    QFont cellFont = baseFont;
    cellFont.setPointSize(8);

    QFont statusFont = baseFont;
    statusFont.setPointSize(8);
    statusFont.setBold(true);

    // ---------------------------------------------------------
    // Status text
    // ---------------------------------------------------------

    auto statusText = [](MaintenanceStatus status)
    {
        switch (status)
        {
        case MaintenanceStatus::Scheduled:
            return QString("برنامه‌ریزی شده");

        case MaintenanceStatus::WaitingForHandling:
            return QString("در انتظار رسیدگی");

        case MaintenanceStatus::InProgress:
            return QString("در حال انجام");

        case MaintenanceStatus::Completed:
            return QString("تکمیل شده");

        case MaintenanceStatus::Cancelled:
            return QString("لغو شده");

        case MaintenanceStatus::Overdue:
            return QString("معوق");

        case MaintenanceStatus::Missed:
            return QString("انجام نشده");
        }

        return QString("نامشخص");
    };

    // ---------------------------------------------------------
    // Draw header
    // ---------------------------------------------------------

    int y = top;

    painter.setFont(titleFont);
    painter.setPen(QColor("#1e293b"));

    painter.drawText(
        QRect(left, y, tableWidth, 35),
        Qt::AlignRight | Qt::AlignVCenter,
        "گزارش نگهداری و تعمیرات"
        );

    y += 38;

    painter.setFont(subtitleFont);
    painter.setPen(QColor("#64748b"));

    const QString generatedText =
        QString("تعداد رویدادها: %1").arg(events.size());

    painter.drawText(
        QRect(left, y, tableWidth, 25),
        Qt::AlignRight | Qt::AlignVCenter,
        generatedText
        );

    y += 35;

    // Separator
    painter.setPen(QColor("#cbd5e1"));
    painter.drawLine(left, y, right, y);

    y += 15;

    // ---------------------------------------------------------
    // Table configuration
    // ---------------------------------------------------------

    const int headerHeight = 32;
    const int rowHeight = 34;

    // Right-to-left visual order.
    //
    // We still use logical column indexes:
    //
    // 0 = title
    // 1 = asset
    // 2 = sub asset
    // 3 = location
    // 4 = date
    // 5 = status
    //
    // But draw them from right to left.

    const QList<int> columnWidths =
        {
            210, // title
            150, // asset
            130, // sub asset
            145, // location
            100, // date
            120  // status
        };

    const QStringList headers =
        {
            "عنوان رویداد",
            "تجهیز",
            "زیر تجهیز",
            "محل نصب",
            "تاریخ",
            "وضعیت"
        };

    // ---------------------------------------------------------
    // Draw table header
    // ---------------------------------------------------------

    auto drawTableHeader = [&]()
    {
        painter.setFont(headerFont);

        int x = right;

        for (int i = 0; i < headers.size(); ++i)
        {
            const int width = columnWidths[i];

            x -= width;

            QRect rect(x, y, width, headerHeight);

            painter.fillRect(
                rect,
                QColor("#e2e8f0")
                );

            painter.setPen(QColor("#334155"));

            painter.drawRect(rect);

            painter.drawText(
                rect.adjusted(8, 0, -8, 0),
                Qt::AlignCenter,
                headers[i]
                );
        }

        y += headerHeight;
    };

    drawTableHeader();

    // ---------------------------------------------------------
    // Draw rows
    // ---------------------------------------------------------

    painter.setFont(cellFont);

    for (int index = 0; index < events.size(); ++index)
    {
        const MaintenanceEventData& event = events[index];

        // New page
        if (y + rowHeight > bottom)
        {
            writer.newPage();

            y = top;

            painter.setFont(titleFont);
            painter.setPen(QColor("#1e293b"));

            painter.drawText(
                QRect(left, y, tableWidth, 35),
                Qt::AlignRight | Qt::AlignVCenter,
                "گزارش نگهداری و تعمیرات"
                );

            y += 50;

            drawTableHeader();

            painter.setFont(cellFont);
        }

        const QColor rowBackground =
            (index % 2 == 0)
                ? QColor("#ffffff")
                : QColor("#f8fafc");

        int x = right;

        const QStringList values =
            {
                event.title,
                event.assetName,
                event.subAssetName,
                event.installationPlace,
                event.scheduledDate.toString("yyyy/MM/dd"),
                statusText(event.status)
            };

        for (int column = 0; column < values.size(); ++column)
        {
            const int width = columnWidths[column];

            x -= width;

            QRect rect(x, y, width, rowHeight);

            painter.fillRect(
                rect,
                rowBackground
                );

            painter.setPen(QColor("#e2e8f0"));
            painter.drawRect(rect);

            // -------------------------------------------------
            // Status cell
            // -------------------------------------------------

            if (column == 5)
            {
                QColor background;
                QColor foreground;

                switch (event.status)
                {
                case MaintenanceStatus::Scheduled:
                case MaintenanceStatus::WaitingForHandling:
                case MaintenanceStatus::InProgress:
                    background = QColor("#eff6ff");
                    foreground = QColor("#2563eb");
                    break;

                case MaintenanceStatus::Completed:
                    background = QColor("#ecfdf5");
                    foreground = QColor("#059669");
                    break;

                case MaintenanceStatus::Cancelled:
                    background = QColor("#fef2f2");
                    foreground = QColor("#dc2626");
                    break;

                case MaintenanceStatus::Overdue:
                    background = QColor("#fff7ed");
                    foreground = QColor("#ea580c");
                    break;

                case MaintenanceStatus::Missed:
                    background = QColor("#f1f5f9");
                    foreground = QColor("#475569");
                    break;

                default:
                    background = QColor("#f8fafc");
                    foreground = QColor("#475569");
                    break;
                }

                const QRect badgeRect =
                    rect.adjusted(10, 7, -10, -7);

                painter.setPen(Qt::NoPen);
                painter.setBrush(background);

                painter.drawRoundedRect(
                    badgeRect,
                    6,
                    6
                    );

                painter.setFont(statusFont);
                painter.setPen(foreground);

                painter.drawText(
                    badgeRect,
                    Qt::AlignCenter,
                    values[column]
                    );

                painter.setFont(cellFont);
            }
            else
            {
                painter.setPen(QColor("#334155"));

                painter.drawText(
                    rect.adjusted(8, 0, -8, 0),
                    Qt::AlignVCenter | Qt::AlignRight,
                    values[column]
                    );
            }
        }

        y += rowHeight;
    }

    // ---------------------------------------------------------
    // Empty report
    // ---------------------------------------------------------

    if (events.isEmpty())
    {
        QRect emptyRect(
            left,
            y,
            tableWidth,
            70
            );

        painter.setPen(QColor("#94a3b8"));
        painter.setFont(subtitleFont);

        painter.drawText(
            emptyRect,
            Qt::AlignCenter,
            "رویدادی برای نمایش وجود ندارد."
            );
    }

    // ---------------------------------------------------------
    // Footer
    // ---------------------------------------------------------

    painter.setFont(subtitleFont);
    painter.setPen(QColor("#94a3b8"));

    const QRect footerRect(
        left,
        pageRect.bottom() - 30,
        tableWidth,
        20
        );

    painter.drawText(
        footerRect,
        Qt::AlignCenter,
        QString("گزارش نگهداری و تعمیرات")
            .arg(writer.pageLayout().pageSize().isValid() ? "" : "")
        );

    painter.end();

    return true;
}

void ReportsSection::setReportDateRange(
    const QDate& fromDate,
    const QDate& toDate
    )
{
    if (!fromDate.isValid() || !toDate.isValid())
        return;

    if (fromDate > toDate)
        return;

    m_fromDateEdit->setDate(fromDate);
    m_toDateEdit->setDate(toDate);
}
void ReportsSection::setCurrentWeekRange()
{
    const QDate today = QDate::currentDate();

    // QDate::dayOfWeek():
    // Monday = 1 ... Friday = 5, Saturday = 6, Sunday = 7
    //
    // Convert it so Saturday becomes 0:
    // Saturday -> 0
    // Sunday   -> 1
    // Monday   -> 2
    // ...
    // Friday   -> 6
    const int daysSinceSaturday =
        (today.dayOfWeek() + 1) % 7;

    const QDate saturday =
        today.addDays(-daysSinceSaturday);

    setReportDateRange(saturday, today);
}

void ReportsSection::setCurrentMonthRange()
{
    const QDate today = QDate::currentDate();

    const QCalendar jalali(QCalendar::System::Jalali);

    const QCalendar::YearMonthDay parts =
        jalali.partsFromDate(today);

    const QDate firstDay =
        jalali.dateFromParts(
            parts.year,
            parts.month,
            1
            );

    setReportDateRange(firstDay, today);
}

void ReportsSection::setLastSixMonthsRange()
{
    const QDate today = QDate::currentDate();

    const QCalendar jalali(QCalendar::System::Jalali);

    const QCalendar::YearMonthDay parts =
        jalali.partsFromDate(today);

    int targetYear = parts.year;
    int targetMonth = parts.month - 6;

    if (targetMonth <= 0)
    {
        targetMonth += 12;
        --targetYear;
    }

    // Start with the same Jalali day-of-month.
    // If that day doesn't exist in the target month,
    // move backwards until we reach a valid date.
    int targetDay = parts.day;

    QDate fromDate;

    while (targetDay >= 1)
    {
        fromDate =
            jalali.dateFromParts(
                targetYear,
                targetMonth,
                targetDay
                );

        if (fromDate.isValid())
            break;

        --targetDay;
    }

    if (!fromDate.isValid())
        return;

    setReportDateRange(fromDate, today);
}

void ReportsSection::setCurrentYearRange()
{
    const QDate today = QDate::currentDate();

    const QCalendar jalali(QCalendar::System::Jalali);

    const QCalendar::YearMonthDay parts =
        jalali.partsFromDate(today);

    const QDate firstDay =
        jalali.dateFromParts(
            parts.year,
            1,
            1
            );

    setReportDateRange(firstDay, today);
}


void ReportsSection::populateAssetFilter()
{

    QHash<QUuid, bool> previousSelections;

    for (QCheckBox* checkBox : m_assetChecks)
    {
        previousSelections.insert(
            checkBox->property("assetId").toUuid(),
            checkBox->isChecked()
            );
    }

    // Remove the previous asset checkboxes.
    while (QLayoutItem* item = m_assetListLayout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            widget->deleteLater();

        delete item;
    }

    m_assetChecks.clear();

    const AssetManager& assetManager =
        m_maintenanceManager.m_assetManager;

    for (const Asset& asset : assetManager.assets())
    {
        auto* checkBox = new QCheckBox(
            asset.info().name,
            m_assetListWidget
            );

        const QUuid assetId = asset.id();

        checkBox->setChecked(
            previousSelections.contains(assetId)
                ? previousSelections.value(assetId)
                : true
            );

        checkBox->setProperty(
            "assetId",
            QVariant::fromValue(asset.id())
            );

        m_assetListLayout->addWidget(checkBox);
        m_assetChecks.append(checkBox);

        connect(
            checkBox,
            &QCheckBox::stateChanged,
            this,
            [this](int)
            {
                int checkedCount = 0;

                for (QCheckBox* item : m_assetChecks)
                {
                    if (item->isChecked())
                        ++checkedCount;
                }

                Qt::CheckState state;

                if (checkedCount == 0)
                    state = Qt::Unchecked;
                else if (checkedCount == m_assetChecks.size())
                    state = Qt::Checked;
                else
                    state = Qt::PartiallyChecked;

                const QSignalBlocker blocker(m_allAssetsCheckBox);
                m_allAssetsCheckBox->setCheckState(state);
            }
            );
    }

    connect(
        m_allAssetsCheckBox,
        &QCheckBox::clicked,
        this,
        [this](bool checked)
        {
            for (QCheckBox* item : m_assetChecks)
                item->setChecked(checked);
        }
        );

    m_allAssetsCheckBox->setCheckState(
        Qt::Checked
        );
}

void ReportsSection::refreshAssets()
{
    populateAssetFilter();
}