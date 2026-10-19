#include "maintenanceeventwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMenu>
#include <QAction>
#include <QIcon>

#include "persiandigits.h"

MaintenanceEventWidget::MaintenanceEventWidget(
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

void MaintenanceEventWidget::setupUI()
{
    setObjectName("maintenanceEventWidget");
    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(
        "#maintenanceEventWidget {"
        "    background-color: #ffffff;"
        "    border: 1px solid #dce4f2;"
        "    border-radius: 12px;"
        "}"
        );
    setLayoutDirection(Qt::RightToLeft);
    setFixedHeight(150);
    setMinimumWidth(650);

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(18, 14, 18, 14);
    mainLayout->setSpacing(10);

    // ========================================================
    // HEADER
    // ========================================================

    auto* headerWidget = new QWidget(this);

    auto* headerLayout = new QHBoxLayout(headerWidget);

    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(12);
    // headerLayout->setDirection(QBoxLayout::RightToLeft);
    // --------------------------------------------------------
    // Event icon
    // --------------------------------------------------------

    auto* iconLabel = new QLabel(headerWidget);

    iconLabel->setFixedSize(42, 42);
    iconLabel->setAlignment(Qt::AlignCenter);

    iconLabel->setPixmap(
        QIcon(":/qlementine/icons/custome/one-gear-blue.svg")
            .pixmap(25, 25)
        );

    iconLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #e9efff;"
        "    border-radius: 12px;"
        "}"
        );

    // --------------------------------------------------------
    // Title
    // --------------------------------------------------------

    auto* titleWidget = new QWidget(headerWidget);

    auto* titleLayout = new QVBoxLayout(titleWidget);

    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(1);

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
        "    color: #718096;"
        "}"
        );

    titleLayout->addWidget(m_titleLabel);
    titleLayout->addWidget(m_typeLabel);

    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(titleWidget, 1);

    // --------------------------------------------------------
    // Status
    // --------------------------------------------------------

    m_statusLabel = new QLabel(headerWidget);

    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setMinimumWidth(105);
    m_statusLabel->setFixedHeight(32);

    headerLayout->addWidget(m_statusLabel);

    // --------------------------------------------------------
    // Menu button
    // --------------------------------------------------------

    m_menuButton = new QPushButton(headerWidget);

    m_menuButton->setFixedSize(34, 34);
    m_menuButton->setIcon(
        QIcon(":/qlementine/icons/custome/menu-dots-horizontal.svg")
        );
    m_menuButton->setIconSize(QSize(18, 18));

    m_menuButton->setFocusPolicy(Qt::NoFocus);

    m_menuButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #eef3ff;"
        "    border: none;"
        "    border-radius: 17px;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #dfe8ff;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #d3dfff;"
        "}"
        );

    headerLayout->addWidget(m_menuButton);

    mainLayout->addWidget(headerWidget);

    // ========================================================
    // SEPARATOR
    // ========================================================

    auto* separator = new QFrame(this);

    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);

    separator->setStyleSheet(
        "QFrame {"
        "    color: #e5eaf5;"
        "}"
        );

    mainLayout->addWidget(separator);

    // ========================================================
    // INFORMATION ROW
    // ========================================================

    auto* infoWidget = new QWidget(this);

    auto* infoLayout = new QHBoxLayout(infoWidget);

    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(0);
    infoLayout->setDirection(QBoxLayout::LeftToRight);

    infoLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/calendar.svg",
            "تاریخ اجرا",
            m_dateLabel
            ),
        1
        );

    infoLayout->addWidget(
        createInfoSeparator(infoWidget)
        );

    infoLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/location.svg",
            "محل نصب",
            m_placeLabel
            ),
        1
        );

    infoLayout->addWidget(
        createInfoSeparator(infoWidget)
        );

    infoLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/one-gear.svg",
            "زیرتجهیز",
            m_subAssetLabel
            ),
        1
        );

    infoLayout->addWidget(
        createInfoSeparator(infoWidget)
        );

    infoLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/one-gear.svg",
            "تجهیز",
            m_assetLabel
            ),
        1
        );

    mainLayout->addWidget(infoWidget);

    setupActionMenu();
    // ========================================================
    // CONNECTIONS
    // ========================================================

    connect(
        m_menuButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (m_actionMenu->isVisible())
            {
                m_actionMenu->hide();
                return;
            }

            QPoint pos =
                m_menuButton->mapToGlobal(
                    QPoint(
                        -m_actionMenu->width() - 6,
                        0
                        )
                    );

            m_actionMenu->move(pos);
            m_actionMenu->show();
        }
        );
}


// ============================================================
// Info item
// ============================================================

QWidget* MaintenanceEventWidget::createInfoItem(
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

    auto* titleLabel = new QLabel(title, titleRow);

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

    valueLabel = new QLabel("-", valueRow);

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

void MaintenanceEventWidget::updateUI()
{
    m_titleLabel->setText(
        m_eventData.title
        );

    m_typeLabel->setText(
        ""
        );

    m_assetLabel->setText(
        m_eventData.assetName
        );

    m_subAssetLabel->setText(
        m_eventData.subAssetName.isEmpty()
            ? "-"
            : m_eventData.subAssetName
        );

    m_placeLabel->setText(
        m_eventData.installationPlace.isEmpty()
            ? "-"
            : m_eventData.installationPlace
        );

    m_dateLabel->setText(
        toPersianDate(m_eventData.scheduledDate)
        );

    updateStatus();
}


// ============================================================
// Status
// ============================================================

void MaintenanceEventWidget::updateStatus()
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
// Background
// ============================================================

void MaintenanceEventWidget::updateBackground()
{
    QString backgroundColor;

    switch (m_eventData.status)
    {
    case MaintenanceStatus::Scheduled:
        backgroundColor = "#ffffff";
        break;

    case MaintenanceStatus::Completed:
        backgroundColor = "#f7fcf8";
        break;

    case MaintenanceStatus::Overdue:
        backgroundColor = "#fffaf0";
        break;

    case MaintenanceStatus::InProgress:
        backgroundColor = "#f7fbff";
        break;

    case MaintenanceStatus::Cancelled:
        backgroundColor = "#f7f7f7";
        break;

    case MaintenanceStatus::Missed:
        backgroundColor = "#fff7f7";
        break;

    case MaintenanceStatus::WaitingForHandling:
        backgroundColor = "#faf8ff";
        break;
    }

    setStyleSheet(
        QString(
            "#maintenanceEventWidget {"
            "    background-color: %1;"
            "    border: 1px solid #dce4f2;"
            "    border-radius: 12px;"
            "}"
            ).arg(backgroundColor)
        );
}


// ============================================================
// Action menu
// ============================================================

void MaintenanceEventWidget::showActionMenu()
{
    QMenu menu(this);

    menu.setLayoutDirection(Qt::RightToLeft);

    menu.setStyleSheet(
        "QMenu {"
        "    background-color: #ffffff;"
        "    color: #4b5563;"
        "    border: 1px solid #dce4f2;"
        "    border-radius: 10px;"
        "    padding: 6px;"
        "}"
        ""
        "QMenu::item {"
        "    color: #4b5563;"
        "    padding: 9px 14px;"
        "    border-radius: 6px;"
        "}"
        ""
        "QMenu::item:selected {"
        "    background-color: #edf3ff;"
        "    color: #1f2937;"
        "}"
        );
    QAction* completeAction =
        menu.addAction(
            QIcon(":/qlementine/icons/custome/check-tick-blue.svg"),
            "تکمیل رویداد"
            );

    QAction* waitingAction =
        menu.addAction(
            QIcon(":/qlementine/icons/16/misc/clock.svg"),
            "در انتظار رسیدگی"
            );

    QAction* editAction =
        menu.addAction(
            QIcon(":/qlementine/icons/16/misc/pen.svg"),
            "ویرایش"
            );

    menu.addSeparator();

    QAction* cancelAction =
        menu.addAction(
            QIcon(":/qlementine/icons/custome/close-red.svg"),
            "لغو رویداد"
            );

    QAction* deleteAction =
        menu.addAction(
            QIcon(":/qlementine/icons/custome/trash-red.svg"),
            "حذف رویداد"
            );

    connect(
        completeAction,
        &QAction::triggered,
        this,
        &MaintenanceEventWidget::onCompleteClicked
        );

    connect(
        waitingAction,
        &QAction::triggered,
        this,
        &MaintenanceEventWidget::onWaitingForHandlingClicked
        );

    connect(
        editAction,
        &QAction::triggered,
        this,
        &MaintenanceEventWidget::onEditClicked
        );

    connect(
        cancelAction,
        &QAction::triggered,
        this,
        &MaintenanceEventWidget::onCancelClicked
        );

    connect(
        deleteAction,
        &QAction::triggered,
        this,
        &MaintenanceEventWidget::onDeleteClicked
        );

    menu.adjustSize();

    QPoint menuPosition =
        m_menuButton->mapToGlobal(QPoint(0, 0));

    menuPosition.setX(
        menuPosition.x() - menu.sizeHint().width()
        );

    menu.exec(menuPosition);
}


// ============================================================
// Actions
// ============================================================


void MaintenanceEventWidget::setupActionMenu()
{
    m_actionMenu =
        new QFrame(
            nullptr,
            Qt::Popup |
                Qt::FramelessWindowHint
            );

    m_actionMenu->setFixedSize(
        210,
        250
        );

    m_actionMenu->setObjectName(
        "eventActionMenu"
        );

    m_actionMenu->setLayoutDirection(
        Qt::RightToLeft
        );

    m_actionMenu->setStyleSheet(
        "QFrame#eventActionMenu {"
        "    background-color: white;"
        "    border: 1px solid #e8ebf2;"
        "    border-radius: 12px;"
        "}"
        );

    auto* menuLayout =
        new QVBoxLayout(m_actionMenu);

    menuLayout->setContentsMargins(
        6, 6, 6, 6
        );

    menuLayout->setSpacing(1);


    // =========================================================
    // COMPLETE
    // =========================================================

    m_completeButton =
        new QPushButton(m_actionMenu);

    m_completeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #f5f7fb;"
        "    border-radius: 8px;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #edf1f7;"
        "    border-radius: 8px;"
        "}"
        );

    m_completeButton->setFixedHeight(45);
    m_completeButton->setCursor(
        Qt::PointingHandCursor
        );
    m_completeButton->setFocusPolicy(
        Qt::NoFocus
        );

    auto* completeLayout =
        new QHBoxLayout(m_completeButton);

    completeLayout->setContentsMargins(
        6, 3, 6, 3
        );

    completeLayout->setSpacing(8);

    auto* completeIcon =
        new QLabel(m_completeButton);

    completeIcon->setFixedSize(32, 32);
    completeIcon->setAlignment(Qt::AlignCenter);

    completeIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/custome/check-tick-blue.svg"
            ).pixmap(17, 17)
        );

    completeIcon->setStyleSheet(
        "background: #edf6ff;"
        "border: none;"
        "border-radius: 16px;"
        );

    completeLayout->addWidget(
        completeIcon
        );

    auto* completeTextLayout =
        new QVBoxLayout();

    completeTextLayout->setContentsMargins(
        0, 0, 0, 0
        );

    completeTextLayout->setSpacing(0);

    auto* completeTitle =
        new QLabel(
            "تکمیل رویداد",
            m_completeButton
            );

    completeTitle->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #17213c;"
        "    font-size: 11px;"
        "    font-weight: 700;"
        "}"
        );

    auto* completeDescription =
        new QLabel(
            "ثبت انجام شدن رویداد",
            m_completeButton
            );

    completeDescription->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #8a91a3;"
        "    font-size: 8px;"
        "}"
        );

    completeTextLayout->addWidget(
        completeTitle
        );

    completeTextLayout->addWidget(
        completeDescription
        );

    completeLayout->addLayout(
        completeTextLayout,
        1
        );

    menuLayout->addWidget(
        m_completeButton
        );


    // =========================================================
    // WAITING FOR HANDLING
    // =========================================================

    m_waitingButton =
        new QPushButton(m_actionMenu);

    m_waitingButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #f5f7fb;"
        "    border-radius: 8px;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #edf1f7;"
        "    border-radius: 8px;"
        "}"
        );

    m_waitingButton->setFixedHeight(45);
    m_waitingButton->setCursor(
        Qt::PointingHandCursor
        );
    m_waitingButton->setFocusPolicy(
        Qt::NoFocus
        );

    auto* waitingLayout =
        new QHBoxLayout(m_waitingButton);

    waitingLayout->setContentsMargins(
        6, 3, 6, 3
        );

    waitingLayout->setSpacing(8);

    auto* waitingIcon =
        new QLabel(m_waitingButton);

    waitingIcon->setFixedSize(32, 32);
    waitingIcon->setAlignment(Qt::AlignCenter);

    waitingIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/16/misc/clock.svg"
            ).pixmap(17, 17)
        );

    waitingIcon->setStyleSheet(
        "background: #f1ebff;"
        "border: none;"
        "border-radius: 16px;"
        );

    waitingLayout->addWidget(
        waitingIcon
        );

    auto* waitingTextLayout =
        new QVBoxLayout();

    waitingTextLayout->setContentsMargins(
        0, 0, 0, 0
        );

    waitingTextLayout->setSpacing(0);

    auto* waitingTitle =
        new QLabel(
            "در انتظار رسیدگی",
            m_waitingButton
            );

    waitingTitle->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #17213c;"
        "    font-size: 11px;"
        "    font-weight: 700;"
        "}"
        );

    auto* waitingDescription =
        new QLabel(
            "قرار دادن رویداد در صف رسیدگی",
            m_waitingButton
            );

    waitingDescription->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #8a91a3;"
        "    font-size: 8px;"
        "}"
        );

    waitingTextLayout->addWidget(
        waitingTitle
        );

    waitingTextLayout->addWidget(
        waitingDescription
        );

    waitingLayout->addLayout(
        waitingTextLayout,
        1
        );

    menuLayout->addWidget(
        m_waitingButton
        );


    // =========================================================
    // SEPARATOR
    // =========================================================

    auto* separator =
        new QFrame(m_actionMenu);

    separator->setFixedHeight(1);

    separator->setStyleSheet(
        "background-color: #edf0f5;"
        "border: none;"
        );

    menuLayout->addWidget(
        separator
        );


    // =========================================================
    // EDIT
    // =========================================================

    m_editButton =
        new QPushButton(m_actionMenu);

    m_editButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #f5f7fb;"
        "    border-radius: 8px;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #edf1f7;"
        "    border-radius: 8px;"
        "}"
        );

    m_editButton->setFixedHeight(45);
    m_editButton->setCursor(
        Qt::PointingHandCursor
        );
    m_editButton->setFocusPolicy(
        Qt::NoFocus
        );

    auto* editLayout =
        new QHBoxLayout(m_editButton);

    editLayout->setContentsMargins(
        6, 3, 6, 3
        );

    editLayout->setSpacing(8);

    auto* editIcon =
        new QLabel(m_editButton);

    editIcon->setFixedSize(32, 32);
    editIcon->setAlignment(Qt::AlignCenter);

    editIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/16/misc/pen.svg"
            ).pixmap(17, 17)
        );

    editIcon->setStyleSheet(
        "background: #f3f4f6;"
        "border: none;"
        "border-radius: 16px;"
        );

    editLayout->addWidget(
        editIcon
        );

    auto* editTextLayout =
        new QVBoxLayout();

    editTextLayout->setContentsMargins(
        0, 0, 0, 0
        );

    editTextLayout->setSpacing(0);

    auto* editTitle =
        new QLabel(
            "ویرایش",
            m_editButton
            );

    editTitle->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #17213c;"
        "    font-size: 11px;"
        "    font-weight: 700;"
        "}"
        );

    auto* editDescription =
        new QLabel(
            "ویرایش اطلاعات رویداد",
            m_editButton
            );

    editDescription->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #8a91a3;"
        "    font-size: 8px;"
        "}"
        );

    editTextLayout->addWidget(editTitle);
    editTextLayout->addWidget(editDescription);

    editLayout->addLayout(
        editTextLayout,
        1
        );

    menuLayout->addWidget(
        m_editButton
        );


    // =========================================================
    // SEPARATOR
    // =========================================================

    auto* separator2 =
        new QFrame(m_actionMenu);

    separator2->setFixedHeight(1);

    separator2->setStyleSheet(
        "background-color: #edf0f5;"
        "border: none;"
        );

    menuLayout->addWidget(
        separator2
        );


    // =========================================================
    // CANCEL
    // =========================================================

    m_cancelButton =
        new QPushButton(m_actionMenu);

    m_cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #f5f7fb;"
        "    border-radius: 8px;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #edf1f7;"
        "    border-radius: 8px;"
        "}"
        );
    m_cancelButton->setFixedHeight(45);
    m_cancelButton->setCursor(
        Qt::PointingHandCursor
        );
    m_cancelButton->setFocusPolicy(
        Qt::NoFocus
        );

    auto* cancelLayout =
        new QHBoxLayout(m_cancelButton);

    cancelLayout->setContentsMargins(
        6, 3, 6, 3
        );

    cancelLayout->setSpacing(8);

    auto* cancelIcon =
        new QLabel(m_cancelButton);

    cancelIcon->setFixedSize(32, 32);
    cancelIcon->setAlignment(Qt::AlignCenter);

    cancelIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/custome/close-red.svg"
            ).pixmap(17, 17)
        );

    cancelIcon->setStyleSheet(
        "background: #fff0f1;"
        "border: none;"
        "border-radius: 16px;"
        );

    cancelLayout->addWidget(
        cancelIcon
        );

    auto* cancelTextLayout =
        new QVBoxLayout();

    cancelTextLayout->setContentsMargins(
        0, 0, 0, 0
        );

    cancelTextLayout->setSpacing(0);

    auto* cancelTitle =
        new QLabel(
            "لغو رویداد",
            m_cancelButton
            );

    cancelTitle->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #dc3545;"
        "    font-size: 11px;"
        "    font-weight: 700;"
        "}"
        );

    auto* cancelDescription =
        new QLabel(
            "لغو اجرای این رویداد",
            m_cancelButton
            );

    cancelDescription->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #d98a91;"
        "    font-size: 8px;"
        "}"
        );

    cancelTextLayout->addWidget(cancelTitle);
    cancelTextLayout->addWidget(cancelDescription);

    cancelLayout->addLayout(
        cancelTextLayout,
        1
        );

    menuLayout->addWidget(
        m_cancelButton
        );


    // =========================================================
    // DELETE
    // =========================================================

    m_deleteButton =
        new QPushButton(m_actionMenu);

    m_deleteButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #f5f7fb;"
        "    border-radius: 8px;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #edf1f7;"
        "    border-radius: 8px;"
        "}"
        );


    m_deleteButton->setFixedHeight(45);
    m_deleteButton->setCursor(
        Qt::PointingHandCursor
        );
    m_deleteButton->setFocusPolicy(
        Qt::NoFocus
        );

    auto* deleteLayout =
        new QHBoxLayout(m_deleteButton);

    deleteLayout->setContentsMargins(
        6, 3, 6, 3
        );

    deleteLayout->setSpacing(8);

    auto* deleteIcon =
        new QLabel(m_deleteButton);

    deleteIcon->setFixedSize(32, 32);
    deleteIcon->setAlignment(Qt::AlignCenter);

    deleteIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/custome/trash-red.svg"
            ).pixmap(17, 17)
        );

    deleteIcon->setStyleSheet(
        "background: #fff0f1;"
        "border: none;"
        "border-radius: 16px;"
        );

    deleteLayout->addWidget(
        deleteIcon
        );

    auto* deleteTextLayout =
        new QVBoxLayout();

    deleteTextLayout->setContentsMargins(
        0, 0, 0, 0
        );

    deleteTextLayout->setSpacing(0);

    auto* deleteTitle =
        new QLabel(
            "حذف رویداد",
            m_deleteButton
            );

    deleteTitle->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #dc3545;"
        "    font-size: 11px;"
        "    font-weight: 700;"
        "}"
        );

    auto* deleteDescription =
        new QLabel(
            "حذف دائمی این رویداد",
            m_deleteButton
            );

    deleteDescription->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #d98a91;"
        "    font-size: 8px;"
        "}"
        );

    deleteTextLayout->addWidget(deleteTitle);
    deleteTextLayout->addWidget(deleteDescription);

    deleteLayout->addLayout(
        deleteTextLayout,
        1
        );

    menuLayout->addWidget(
        m_deleteButton
        );


    // =========================================================
    // BUTTON ACTIONS
    // =========================================================

    connect(
        m_completeButton,
        &QPushButton::clicked,
        this,
        &MaintenanceEventWidget::onCompleteClicked
        );

    connect(
        m_waitingButton,
        &QPushButton::clicked,
        this,
        &MaintenanceEventWidget::onWaitingForHandlingClicked
        );

    connect(
        m_editButton,
        &QPushButton::clicked,
        this,
        &MaintenanceEventWidget::onEditClicked
        );

    connect(
        m_cancelButton,
        &QPushButton::clicked,
        this,
        &MaintenanceEventWidget::onCancelClicked
        );

    connect(
        m_deleteButton,
        &QPushButton::clicked,
        this,
        &MaintenanceEventWidget::onDeleteClicked
        );
}

void MaintenanceEventWidget::onCompleteClicked()
{
    m_actionMenu->hide();

    emit statusChangeRequested(
        m_eventData.id,
        MaintenanceStatus::Completed
        );
}


void MaintenanceEventWidget::onWaitingForHandlingClicked()
{
    m_actionMenu->hide();

    emit statusChangeRequested(
        m_eventData.id,
        MaintenanceStatus::WaitingForHandling
        );
}


void MaintenanceEventWidget::onEditClicked()
{
    m_actionMenu->hide();

    emit editRequested(
        m_eventData.id
        );
}


void MaintenanceEventWidget::onCancelClicked()
{
    m_actionMenu->hide();

    emit statusChangeRequested(
        m_eventData.id,
        MaintenanceStatus::Cancelled
        );
}


void MaintenanceEventWidget::onDeleteClicked()
{
    m_actionMenu->hide();

    emit deleteRequested(
        m_eventData.id
        );
}

QFrame* MaintenanceEventWidget::createInfoSeparator(QWidget* parent)
{
    auto* separator =
        new QFrame(parent);

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