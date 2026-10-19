#include "maintenanceschedulewidget.h"
#include "customicons.h"
#include "persiandigits.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QStyle>
#include <QSizePolicy>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>


MaintenanceScheduleWidget::MaintenanceScheduleWidget(
    const MaintenanceSchedule& schedule,
    QWidget* parent
    )
    : QFrame(parent)
    , m_schedule(schedule)
{
    setupUI();
}

void MaintenanceScheduleWidget::setupUI()
{
    setObjectName("maintenanceScheduleWidget");

    setFixedSize(360, 140);

    setLayoutDirection(Qt::RightToLeft);

    setStyleSheet(
        "QFrame#maintenanceScheduleWidget {"
        "    background-color: white;"
        "    border: 1px solid #e8ebf2;"
        "    border-right: 5px solid #4969e8;"
        "    border-radius: 14px;"
        "}"
        );

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(14, 10, 14, 10);
    mainLayout->setAlignment(Qt::AlignTop);

    mainLayout->setSpacing(0);


    // =========================================================
    // TOP SECTION
    // =========================================================

    auto* topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(5);
    topLayout->setAlignment(Qt::AlignTop);


    // ---------------------------------------------------------
    // Icon
    // ---------------------------------------------------------

    auto* iconFrame = new QFrame(this);

    iconFrame->setFixedSize(55, 55);
    iconFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #eef2ff;"
        "    border: none;"
        "    border-radius: 13px;"
        "}"
        );

    auto* iconLayout = new QVBoxLayout(iconFrame);

    iconLayout->setContentsMargins(0, 0, 0, 0);
    iconLayout->setAlignment(Qt::AlignCenter);

    auto* iconLabel = new QLabel(iconFrame);
    iconLabel->setFixedSize(55, 55);
    iconLabel->setAlignment(Qt::AlignCenter);

    iconLabel->setPixmap(
        QIcon(":/qlementine/icons/custome/calendar-clock.svg").pixmap(55, 55)
        );


    /*  iconLabel->setStyleSheet(
        "font-size: 28px;"
        "font-weight: 300;"
        "color: #4969e8;"
        "background: transparent;"
        "border: none;"
        );
*/
    iconLayout->addWidget(iconLabel);

    // RTL: icon on the right
    topLayout->addWidget(iconFrame, 0);


    // ---------------------------------------------------------
    // Title / Asset
    // ---------------------------------------------------------

    auto* titleLayout = new QVBoxLayout();

    titleLayout->setContentsMargins(0, 10, 0, 0);
    titleLayout->setSpacing(5);

    m_titleLabel =
        new QLabel(m_schedule.title, this);

    titleLayout->setAlignment(Qt::AlignTop);

    m_titleLabel->setWordWrap(false);

    m_titleLabel->setStyleSheet(
        "font-size: 15px;"
        "font-weight: 700;"
        "color: #17213c;"
        "background: transparent;"
        );
    titleLayout->addWidget(m_titleLabel, 0);


    QString assetText =
        m_schedule.subAssetId.isNull()
            ? "تجهیز"
            : "زیرتجهیز";

    m_assetLabel =
        new QLabel(assetText, this);

    titleLayout->setAlignment(Qt::AlignTop);

    m_assetLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #737b91;"
        "background: transparent;"
        "border: none;"
        "padding: 0px;"
        "margin: 0px;"
        );
    m_titleLabel->setSizePolicy(
        QSizePolicy::Preferred,
        QSizePolicy::Fixed
        );

    m_assetLabel->setSizePolicy(
        QSizePolicy::Preferred,
        QSizePolicy::Fixed
        );
    titleLayout->addWidget(m_assetLabel, 0);
    topLayout->addLayout(titleLayout, 0);


    // ---------------------------------------------------------
    // Next schedule
    // ---------------------------------------------------------

    auto* nextLayout = new QVBoxLayout();
    nextLayout->setContentsMargins(0, 25, 0, 0);
    nextLayout->setSpacing(10);

    auto* textLayout = new QHBoxLayout();
    textLayout->setSpacing(10);

    auto* nextIcon = new QLabel();
    nextIcon->setPixmap(QIcon(":/qlementine/icons/custome/calendar.svg").pixmap(12, 12));
    nextIcon->setFixedHeight(12);
    nextIcon->setFixedWidth(12);

    textLayout->addWidget(nextIcon);

    auto* nextTitle =
        new QLabel("نوبت بعدی", this);

    nextTitle->setAlignment(
        Qt::AlignVCenter
        );

    nextTitle->setStyleSheet(
        "font-size: 13px;"
        "color: #737b91;"
        "background: transparent;"
        "border: none;"
        );

    textLayout->addWidget(nextTitle);

    nextLayout->addLayout(textLayout);

    QString nextDateText = "-";

    if (m_schedule.nextScheduledDate.isValid())
    {
        nextDateText =
            toPersianDate(m_schedule.nextScheduledDate);
    }

    m_nextScheduleLabel =
        new QLabel(nextDateText, this);

    m_nextScheduleLabel->setAlignment(
        Qt::AlignRight | Qt::AlignVCenter
        );

    m_nextScheduleLabel->setStyleSheet(
        "font-size: 11px;"
        "font-weight: 600;"
        "color: #17213c;"
        "background: transparent;"
        "border: none;"
        );

    nextLayout->addWidget(
        m_nextScheduleLabel
        );

    // Give this section a predictable width.
    nextLayout->setSizeConstraint(
        QLayout::SetMinimumSize
        );

    topLayout->addLayout(nextLayout, 0);

    setupStatusControl();
    topLayout->addStretch();

    topLayout->addWidget(
        m_statusButton,
        0,
        Qt::AlignTop | Qt::AlignRight
        );

    mainLayout->addLayout(topLayout);


    // =========================================================
    // SEPARATOR
    // =========================================================

    auto* separator =
        new QFrame(this);

    separator->setFixedHeight(2);

    separator->setStyleSheet(
        "background-color: #edf0f5;"
        "border: none;"
        );

    mainLayout->addSpacing(7);
    mainLayout->addWidget(separator);
    mainLayout->addSpacing(6);


    // =========================================================
    // BOTTOM SECTION
    // =========================================================

    auto* bottomLayout =
        new QHBoxLayout();

    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(10);

    // ---------------------------------------------------------
    // RIGHT: Equipment
    // ---------------------------------------------------------

    auto* equipmentSection =
        new QHBoxLayout();

    equipmentSection->setContentsMargins(0, 0, 0, 0);
    equipmentSection->setSpacing(6);

    auto* equipmentIcon =
        new QLabel(this);
    equipmentIcon->setPixmap(
        QIcon(":/qlementine/icons/custome/one-gear.svg").pixmap(18, 18)
        );
    equipmentIcon->setFixedSize(20, 20);
    equipmentIcon->setAlignment(Qt::AlignVCenter);

    equipmentSection->addWidget(
        equipmentIcon
        );

    auto* equipmentTextLayout =
        new QVBoxLayout();

    equipmentTextLayout->setContentsMargins(0, 0, 0, 0);
    equipmentTextLayout->setSpacing(0);

    auto* equipmentTitle =
        new QLabel("تجهیز", this);

    equipmentTitle->setAlignment(
        Qt::AlignVCenter
        );

    equipmentTitle->setStyleSheet(
        "font-size: 9px;"
        "color: #737b91;"
        "background: transparent;"
        "border: none;"
        );

    equipmentTextLayout->addWidget(
        equipmentTitle
        );


    QString equipmentText =
        m_schedule.subAssetId.isNull()
            ? "کل تجهیز"
            : "زیرتجهیز";

    auto* equipmentValue =
        new QLabel(equipmentText, this);

    equipmentValue->setAlignment(
        Qt::AlignVCenter
        );

    equipmentValue->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #17213c;"
        "background: transparent;"
        "border: none;"
        );

    equipmentTextLayout->addWidget(
        equipmentValue
        );

    equipmentSection->addLayout(
        equipmentTextLayout
        );

    // Keep equipment section on the right.
    bottomLayout->addLayout(
        equipmentSection,
        1
        );

    //separator
    auto* lowerSeparator = new QFrame();

    lowerSeparator->setFixedWidth(1);
    lowerSeparator->setSizePolicy(
        QSizePolicy::Fixed,
        QSizePolicy::Expanding
        );

    lowerSeparator->setStyleSheet(
        "background-color: #edf0f5;"
        "border: none;"
        );

    bottomLayout->addWidget(lowerSeparator);
    bottomLayout->addSpacing(5);

    // ---------------------------------------------------------
    // LEFT: Interval
    // ---------------------------------------------------------

    auto* intervalSection =
        new QHBoxLayout();

    intervalSection->setContentsMargins(0, 0, 0, 0);
    intervalSection->setSpacing(6);

    auto* repeatIcon = new QLabel();
    repeatIcon->setPixmap(
        QIcon(":/qlementine/icons/custome/repeat.svg").pixmap(18, 18)
        );
    repeatIcon->setFixedSize(20, 20);
    repeatIcon->setAlignment(Qt::AlignVCenter);

    intervalSection->addWidget(
        repeatIcon
        );

    // Text first, icon on its right because this section is RTL.
    auto* intervalTextLayout =
        new QVBoxLayout();

    intervalTextLayout->setContentsMargins(0, 0, 0, 0);
    intervalTextLayout->setSpacing(0);

    auto* intervalTitle =
        new QLabel("دوره تکرار", this);

    intervalTitle->setAlignment(
        Qt::AlignVCenter
        );

    intervalTitle->setStyleSheet(
        "font-size: 9px;"
        "color: #737b91;"
        "background: transparent;"
        "border: none;"
        );

    intervalTextLayout->addWidget(
        intervalTitle
        );


    m_intervalLabel =
        new QLabel(intervalText(), this);

    m_intervalLabel->setAlignment(
        Qt::AlignVCenter
        );

    m_intervalLabel->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 600;"
        "color: #17213c;"
        "background: transparent;"
        "border: none;"
        );

    intervalTextLayout->addWidget(
        m_intervalLabel
        );

    intervalSection->addLayout(
        intervalTextLayout
        );

    // Keep left section on the left.
    bottomLayout->addLayout(
        intervalSection,
        1
        );


    mainLayout->addLayout(
        bottomLayout
        );

}
QString MaintenanceScheduleWidget::intervalText() const
{
    QString unit;

    switch (m_schedule.intervalType)
    {
    case MaintenanceIntervalType::Days:
        unit = "روز";
        break;

    case MaintenanceIntervalType::Weeks:
        unit = "هفته";
        break;

    case MaintenanceIntervalType::Months:
        unit = "ماه";
        break;

    case MaintenanceIntervalType::Years:
        unit = "سال";
        break;
    }

    return QString("هر %1 %2")
        .arg(m_schedule.interval)
        .arg(unit);
}

void MaintenanceScheduleWidget::setupStatusControl()
{
    // =========================================================
    // STATUS BUTTON
    // =========================================================

    m_statusButton = new QPushButton(this);

    m_statusButton->setFixedSize(92, 30);
    m_statusButton->setCursor(Qt::PointingHandCursor);
    m_statusButton->setFocusPolicy(Qt::NoFocus);
    m_statusButton->setLayoutDirection(Qt::RightToLeft);

    auto* statusLayout =
        new QHBoxLayout(m_statusButton);

    statusLayout->setContentsMargins(8, 3, 7, 3);
    statusLayout->setSpacing(5);


    // ---------------------------------------------------------
    // Status text
    // ---------------------------------------------------------

    m_statusText =
        new QLabel(m_statusButton);

    m_statusText->setAlignment(
        Qt::AlignCenter
        );

    m_statusText->setAttribute(
        Qt::WA_TransparentForMouseEvents
        );

    m_statusText->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #17213c;"
        "    font-size: 11px;"
        "    font-weight: 700;"
        "}"
        );

    statusLayout->addWidget(
        m_statusText,
        1,
        Qt::AlignVCenter
        );


    // ---------------------------------------------------------
    // Chevron
    // ---------------------------------------------------------

    m_statusChevron =
        new RotatableChevron(m_statusButton);

    m_statusChevron->setAttribute(
        Qt::WA_TransparentForMouseEvents
        );

    statusLayout->addWidget(
        m_statusChevron,
        0,
        Qt::AlignVCenter
        );


    // ---------------------------------------------------------
    // Chevron animation
    // ---------------------------------------------------------

    m_chevronAnimation =
        new QPropertyAnimation(
            m_statusChevron,
            "angle",
            this
            );

    m_chevronAnimation->setDuration(160);

    m_chevronAnimation->setEasingCurve(
        QEasingCurve::OutCubic
        );


    // =========================================================
    // FLOATING MENU
    // =========================================================
    m_statusMenu =
        new QFrame(
            nullptr,
            Qt::Popup |
                Qt::FramelessWindowHint
            );

    /*    m_statusMenu->setAttribute(
        Qt::WA_ShowWithoutActivating
        );

    m_statusMenu->setAttribute(
        Qt::WA_TranslucentBackground
        );
    */
    m_statusMenu->setFixedSize(
        200,
        156
        );

    m_statusMenu->setObjectName(
        "scheduleStatusMenu"
        );

    m_statusMenu->setStyleSheet(
        "QFrame#scheduleStatusMenu {"
        "    background-color: white;"
        "    border: 1px solid #e8ebf2;"
        "    border-radius: 14px;"
        "}"
        );

    m_statusMenu->installEventFilter(this);
    m_statusButton->installEventFilter(this);

    auto* menuLayout =
        new QVBoxLayout(m_statusMenu);

    menuLayout->setContentsMargins(
        6, 6, 6, 6
        );

    menuLayout->setSpacing(1);


    // =========================================================
    // PAUSE / RESUME BUTTON
    // =========================================================

    m_pauseResumeButton =
        new QPushButton(m_statusMenu);

    m_pauseResumeButton->setFixedHeight(55);
    m_pauseResumeButton->setCursor(
        Qt::PointingHandCursor
        );
    m_pauseResumeButton->setFocusPolicy(
        Qt::NoFocus
        );

    auto* pauseLayout =
        new QHBoxLayout(
            m_pauseResumeButton
            );

    pauseLayout->setContentsMargins(
        6, 4, 6, 4
        );

    pauseLayout->setSpacing(8);


    m_pauseResumeIconFrame =
        new QFrame(m_pauseResumeButton);

    m_pauseResumeIconFrame->setFixedSize(
        38,
        38
        );


    auto* pauseIconLayout =
        new QHBoxLayout(
            m_pauseResumeIconFrame
            );

    pauseIconLayout->setContentsMargins(
        0, 0, 0, 0
        );

    pauseIconLayout->setAlignment(
        Qt::AlignCenter
        );


    pauseIcon =
        new QLabel(
            m_pauseResumeIconFrame
            );

    pauseIcon->setFixedSize(
        20,
        20
        );

    pauseIcon->setAlignment(
        Qt::AlignCenter
        );

    pauseIcon->setStyleSheet(
        "background: transparent;"
        "border: none;"
        );

    pauseIconLayout->addWidget(
        pauseIcon
        );


    pauseLayout->addWidget(
        m_pauseResumeIconFrame,
        0,
        Qt::AlignVCenter
        );


    auto* pauseTextLayout =
        new QVBoxLayout();

    pauseTextLayout->setContentsMargins(
        0, 0, 0, 0
        );

    pauseTextLayout->setSpacing(0);


    m_pauseResumeTitle =
        new QLabel(
            m_pauseResumeButton
            );

    m_pauseResumeTitle->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #17213c;"
        "    font-size: 12px;"
        "    font-weight: 700;"
        "}"
        );


    m_pauseResumeDescription =
        new QLabel(
            m_pauseResumeButton
            );

    m_pauseResumeDescription->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #8a91a3;"
        "    font-size: 9px;"
        "}"
        );


    pauseTextLayout->addWidget(
        m_pauseResumeTitle
        );

    pauseTextLayout->addWidget(
        m_pauseResumeDescription
        );

    pauseLayout->addLayout(
        pauseTextLayout,
        1
        );

    menuLayout->addWidget(
        m_pauseResumeButton
        );


    // =========================================================
    // SEPARATOR
    // =========================================================

    auto* menuSeparator =
        new QFrame(m_statusMenu);

    menuSeparator->setFixedHeight(1);

    menuSeparator->setStyleSheet(
        "background-color: #edf0f5;"
        "border: none;"
        );

    menuLayout->addWidget(
        menuSeparator
        );


    // =========================================================
    // REMOVE BUTTON
    // =========================================================

    m_removeButton =
        new QPushButton(m_statusMenu);

    m_removeButton->setFixedHeight(55);

    m_removeButton->setCursor(
        Qt::PointingHandCursor
        );

    m_removeButton->setFocusPolicy(
        Qt::NoFocus
        );


    auto* removeLayout =
        new QHBoxLayout(
            m_removeButton
            );

    removeLayout->setContentsMargins(
        6, 4, 6, 4
        );

    removeLayout->setSpacing(8);


    m_removeIconFrame =
        new QFrame(m_removeButton);

    m_removeIconFrame->setFixedSize(
        38,
        38
        );


    auto* removeIconLayout =
        new QHBoxLayout(
            m_removeIconFrame
            );

    removeIconLayout->setContentsMargins(
        0, 0, 0, 0
        );

    removeIconLayout->setAlignment(
        Qt::AlignCenter
        );


    auto* removeIcon =
        new QLabel(
            m_removeIconFrame
            );

    removeIcon->setFixedSize(
        20,
        20
        );

    removeIcon->setAlignment(
        Qt::AlignCenter
        );

    removeIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/custome/trash-red.svg"
            ).pixmap(18, 18)
        );

    removeIcon->setStyleSheet(
        "background: transparent;"
        "border: none;"
        );

    removeIconLayout->addWidget(
        removeIcon
        );


    removeLayout->addWidget(
        m_removeIconFrame,
        0,
        Qt::AlignVCenter
        );


    auto* removeTextLayout =
        new QVBoxLayout();

    removeTextLayout->setContentsMargins(
        0, 0, 0, 0
        );

    removeTextLayout->setSpacing(0);


    m_removeTitle =
        new QLabel(
            "حذف",
            m_removeButton
            );

    m_removeTitle->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #17213c;"
        "    font-size: 12px;"
        "    font-weight: 700;"
        "}"
        );


    m_removeDescription =
        new QLabel(
            "حذف این برنامه",
            m_removeButton
            );

    m_removeDescription->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    border: none;"
        "    color: #8a91a3;"
        "    font-size: 9px;"
        "}"
        );


    removeTextLayout->addWidget(
        m_removeTitle
        );

    removeTextLayout->addWidget(
        m_removeDescription
        );

    removeLayout->addLayout(
        removeTextLayout,
        1
        );

    menuLayout->addWidget(
        m_removeButton
        );


    // =========================================================
    // BUTTON ACTIONS
    // =========================================================

    connect(
        m_pauseResumeButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const auto newStatus =
                m_schedule.status ==
                        MaintenanceScheduleStatus::Active
                    ? MaintenanceScheduleStatus::Paused
                    : MaintenanceScheduleStatus::Active;

            emit statusChangeRequested(
                m_schedule.id,
                newStatus
                );

            m_statusMenu->hide();
        }
        );


    connect(
        m_removeButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            emit removeRequested(m_schedule.id);
            m_statusMenu->hide();
        }
        );

    // =========================================================
    // STATUS BUTTON
    // =========================================================

    connect(
        m_statusButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (m_statusMenu->isVisible())
            {
                m_statusMenu->hide();
                return;
            }

            QPoint pos =
                m_statusButton->mapToGlobal(
                    QPoint(
                        m_statusButton->width()
                            - m_statusMenu->width(),
                        m_statusButton->height() + 6
                        )
                    );

            m_statusMenu->move(pos);
            m_statusMenu->show();

            m_chevronAnimation->stop();

            m_chevronAnimation->setStartValue(
                m_statusChevron->angle()
                );

            m_chevronAnimation->setEndValue(180.0);

            m_chevronAnimation->start();
        }
        );

    updateStatusControl();
}
void MaintenanceScheduleWidget::updateStatusControl()
{
    if (!m_statusButton)
        return;

    // =========================================================
    // ACTIVE
    // =========================================================

    if (m_schedule.status ==
        MaintenanceScheduleStatus::Active)
    {
        m_statusText->setText("● فعال");

        m_statusText->setStyleSheet(
            "QLabel {"
            "    background: transparent;"
            "    border: none;"
            "    color: #159a62;"
            "    font-size: 11px;"
            "    font-weight: 700;"
            "}"
            );

        m_statusButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #e9fbf4;"
            "    color: #17213c;"
            "    border: 1px solid #a9e8cd;"
            "    border-radius: 14px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #def7ed;"
            "}"
            );

        m_pauseResumeTitle->setText(
            "توقف"
            );

        pauseIcon->setPixmap(
            QIcon(
                ":/qlementine/icons/custome/pause-yellow.svg"
                ).pixmap(18, 18)
            );

        m_pauseResumeDescription->setText(
            "غیرفعال کردن این برنامه"
            );

        m_pauseResumeIconFrame->setStyleSheet(
            "QFrame {"
            "    background-color: #fff1cc;"
            "    border: none;"
            "    border-radius: 19px;"
            "}"
            );

        m_pauseResumeButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #fffaf0;"
            "    border: none;"
            "    border-radius: 10px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #fff4dc;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #ffedc4;"
            "}"
            );
    }


    // =========================================================
    // PAUSED
    // =========================================================

    else
    {
        m_statusText->setText("● متوقف");

        m_statusText->setStyleSheet(
            "QLabel {"
            "    background: transparent;"
            "    border: none;"
            "    color: #d99b00;"
            "    font-size: 11px;"
            "    font-weight: 700;"
            "}"
            );

        m_statusButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #fff8e8;"
            "    color: #17213c;"
            "    border: 1px solid #f1d58b;"
            "    border-radius: 14px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #fff1d0;"
            "}"
            );

        m_pauseResumeTitle->setText(
            "ادامه"
            );

        pauseIcon->setPixmap(
            QIcon(
                ":/qlementine/icons/custome/play-small-blue.svg"
                ).pixmap(18, 18)
            );

        m_pauseResumeDescription->setText(
            "فعال کردن این برنامه"
            );

        m_pauseResumeIconFrame->setStyleSheet(
            "QFrame {"
            "    background-color: #e3edff;"
            "    border: none;"
            "    border-radius: 19px;"
            "}"
            );

        m_pauseResumeButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #f3f7ff;"
            "    border: none;"
            "    border-radius: 10px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #e9f0ff;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #dfeaff;"
            "}"
            );
    }


    // =========================================================
    // REMOVE
    // =========================================================

    m_removeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #fff5f6;"
        "    border: none;"
        "    border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ffebee;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #ffe0e4;"
        "}"
        );

    m_removeIconFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #ffe2e6;"
        "    border: none;"
        "    border-radius: 19px;"
        "}"
        );
}

bool MaintenanceScheduleWidget::eventFilter(
    QObject* watched,
    QEvent* event
    )
{
    // =========================================================
    // STATUS BUTTON
    // =========================================================

    if (watched == m_statusButton)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            auto* mouseEvent =
                static_cast<QMouseEvent*>(event);

            if (mouseEvent->button() == Qt::LeftButton &&
                m_statusMenu->isVisible())
            {
                m_ignoreStatusButtonRelease = true;

                m_statusMenu->hide();

                return true;
            }
        }

        if (event->type() == QEvent::MouseButtonRelease)
        {
            auto* mouseEvent =
                static_cast<QMouseEvent*>(event);

            if (mouseEvent->button() == Qt::LeftButton &&
                m_ignoreStatusButtonRelease)
            {
                m_ignoreStatusButtonRelease = false;

                return true;
            }
        }
    }


    // =========================================================
    // STATUS MENU
    // =========================================================

    if (watched == m_statusMenu &&
        event->type() == QEvent::Hide)
    {
        if (m_chevronAnimation &&
            m_statusChevron)
        {
            m_chevronAnimation->stop();

            m_chevronAnimation->setStartValue(
                m_statusChevron->angle()
                );

            m_chevronAnimation->setEndValue(0.0);

            m_chevronAnimation->start();
        }
    }

    return QFrame::eventFilter(
        watched,
        event
        );
}