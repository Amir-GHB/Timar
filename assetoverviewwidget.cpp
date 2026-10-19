#include "assetoverviewwidget.h"
#include "asset.h"
#include "persiandigits.h"

#include <QDate>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QPixmap>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <qpushbutton.h>
#include <QMessageBox>

AssetOverviewWidget::AssetOverviewWidget(QWidget* parent)
    : QWidget(parent)
{

    setupUI();
}

void AssetOverviewWidget::setupUI()
{
    setLayoutDirection(Qt::RightToLeft);

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);

    /*
     * Main content
     *
     * Right:
     *      Asset image + quick information
     *
     * Left:
     *      General information
     *      Description
     */

    auto* contentLayout = new QHBoxLayout;

    contentLayout->setSpacing(16);

    /*
     * ---------------------------------------------------------
     * Right side
     * ---------------------------------------------------------
     */

    auto* rightColumn = new QVBoxLayout;

    rightColumn->setSpacing(12);

    // Asset image
    auto* imageCard = new QFrame(this);

    imageCard->setObjectName("assetImageCard");

    imageCard->setStyleSheet(
        "QFrame#assetImageCard {"
        "    background-color: white;"
        "    border: 1px solid #e4e9f0;"
        "    border-radius: 14px;"
        "}"
        );

    auto* imageLayout = new QVBoxLayout(imageCard);

    imageLayout->setContentsMargins(12, 12, 12, 12);

    m_assetImageLabel = new QLabel(imageCard);

    m_assetImageLabel->setAlignment(Qt::AlignCenter);
    m_assetImageLabel->setMinimumSize(260, 210);
    m_assetImageLabel->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );

    m_assetImageLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #f5f7fa;"
        "    border-radius: 10px;"
        "}"
        );

    m_assetImageLabel->setText("تصویر تجهیز");

    imageLayout->addWidget(m_assetImageLabel);

    rightColumn->addWidget(imageCard, 1);

    // Quick information
    rightColumn->addWidget(
        createQuickInfoCard(this)
        );

    /*
     * ---------------------------------------------------------
     * Left side
     * ---------------------------------------------------------
     */

    auto* leftColumn = new QVBoxLayout;

    leftColumn->setSpacing(12);

    leftColumn->addWidget(
        createInfoCard(this),
        1
        );

    /*
     * Add columns
     */

    contentLayout->addLayout(leftColumn, 3);
    contentLayout->addLayout(rightColumn, 2);

    mainLayout->addLayout(contentLayout, 1);

    /*
     * ---------------------------------------------------------
     * Overall stylesheet
     * ---------------------------------------------------------
     */

    setStyleSheet(
        "AssetOverviewWidget {"
        "    background-color: #f7f9fc;"
        "}"

        "QFrame#infoCard {"
        "    background-color: white;"
        "    border: 1px solid #e4e9f0;"
        "    border-radius: 14px;"
        "}"

        "QFrame#quickInfoCard {"
        "    background-color: white;"
        "    border: 1px solid #e4e9f0;"
        "    border-radius: 14px;"
        "}"

        "QLabel#sectionTitle {"
        "color: #172033;"
        "font-size: 14px;"
        "font-weight: 600;"
        "}"

        "QLabel#fieldTitle {"
        "    color: #7a8494;"
        "    font-size: 11px;"
        "}"

        "QLabel#fieldValue {"
        "    color: #172033;"
        "    font-size: 13px;"
        "}"

        "QLabel#statusLabel {"
        "    color: #159570;"
        "    font-size: 12px;"
        "    font-weight: 600;"
        "}"
        );
}


QWidget* AssetOverviewWidget::createInfoCard(QWidget* parent)
{
    auto* card = new QFrame(parent);

    card->setObjectName("infoCard");
    //card->setLayoutDirection(Qt::RightToLeft);

    auto* layout = new QVBoxLayout(card);

    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);

    // Header
    auto* sectionTitle = new QLabel(
        "اطلاعات اصلی",
        card
        );

    sectionTitle->setObjectName("sectionTitle");
    sectionTitle->setAlignment(Qt::AlignLeft);
    // sectionTitle->setLayoutDirection(Qt::RightToLeft);

    layout->addWidget(sectionTitle);

    //edit button
    auto* topBar = new QHBoxLayout;
    topBar->setContentsMargins(0, 0, 0, 0);

    auto* deleteButton = new QPushButton(
        "حذف تجهیز",
        this
        );

    deleteButton->setIcon(
        QIcon(":/qlementine/icons/16/action/trash.svg")
        );

    deleteButton->setIconSize(QSize(18, 18));
    deleteButton->setFixedHeight(36);

    connect(
        deleteButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const auto result =
                QMessageBox::warning(
                    this,
                    "حذف تجهیز",
                    "آیا از حذف این تجهیز مطمئن هستید؟\n\n"
                    "تمام رویدادها و برنامه‌های تعمیراتی "
                    "مرتبط با این تجهیز نیز حذف خواهند شد.",
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No
                    );

            if (result == QMessageBox::Yes)
            {
                emit deleteRequested(m_assetId);
            }
        }
        );


    auto* editButton = new QPushButton(
        "ویرایش تجهیز",
        this
        );

    editButton->setIcon(
        QIcon(":/qlementine/icons/16/misc/pen.svg")
        );

    editButton->setIconSize(QSize(18, 18));
    editButton->setFixedHeight(36);

    connect(
        editButton,
        &QPushButton::clicked,
        this,
        &AssetOverviewWidget::editRequested
        );

    topBar->addWidget(deleteButton);
    topBar->addWidget(editButton);
    topBar->addStretch();

    layout->addLayout(topBar);

    // Separator
    auto* separator = new QFrame(card);

    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);

    separator->setStyleSheet(
        "QFrame {"
        "    color: #edf0f4;"
        "}"
        );

    layout->addWidget(separator);

    // Main information rows

    layout->addWidget(
        createInfoRow(
            "نام تجهیز",
            m_assetNameLabel,
            ":/qlementine/icons/custome/one-gear.svg",
            card
            )
        );

    layout->addWidget(
        createInfoRow(
            "نوع تجهیز",
            m_assetTypeLabel,
            ":/qlementine/icons/custome/one-gear.svg",
            card
            )
        );

    layout->addWidget(
        createInfoRow(
            "وضعیت",
            m_statusLabel,
            ":/qlementine/icons/custome/one-gear.svg",
            card
            )
        );

    layout->addWidget(
        createInfoRow(
            "شماره سریال",
            m_serialNumberLabel,
            ":/qlementine/icons/custome/serial-number.svg",
            card
            )
        );

    layout->addWidget(
        createInfoRow(
            "سازنده",
            m_manufacturerLabel,
            ":/qlementine/icons/custome/user.svg",
            card
            )
        );

    layout->addWidget(
        createInfoRow(
            "مدل",
            m_modelLabel,
            ":/qlementine/icons/custome/tag.svg",
            card
            )
        );

    layout->addWidget(
        createInfoRow(
            "تاریخ نصب",
            m_installationDateLabel,
            ":/qlementine/icons/custome/calendar.svg",
            card
            )
        );

    layout->addWidget(
        createInfoRow(
            "محل نصب",
            m_installationPlaceLabel,
            ":/qlementine/icons/custome/location.svg",
            card
            )
        );

    // Description
    auto* descriptionTitle =
        new QLabel("توضیحات", card);

    descriptionTitle->setObjectName("fieldTitle");
    descriptionTitle->setAlignment(Qt::AlignLeft);

    layout->addWidget(descriptionTitle);

    m_descriptionLabel =
        new QLabel("-", card);

    m_descriptionLabel->setObjectName("fieldValue");
    m_descriptionLabel->setLayoutDirection(
        Qt::RightToLeft
        );

    m_descriptionLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignTop
        );

    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setMinimumHeight(60);

    layout->addWidget(
        m_descriptionLabel
        );

    layout->addStretch();

    return card;
}


QWidget* AssetOverviewWidget::createQuickInfoCard(
    QWidget* parent
    )
{
    auto* card = new QFrame(parent);

    card->setObjectName(
        "quickInfoCard"
        );

    card->setLayoutDirection(
        Qt::RightToLeft
        );

    auto* layout =
        new QVBoxLayout(card);

    layout->setContentsMargins(
        18, 16, 18, 16
        );

    layout->setSpacing(12);

    // Header
    auto* title =
        new QLabel(
            "اطلاعات سریع",
            card
            );

    title->setObjectName(
        "sectionTitle"
        );

    title->setLayoutDirection(
        Qt::RightToLeft
        );

    title->setAlignment(
        Qt::AlignLeft
        );

    layout->addWidget(title);

    // Separator
    auto* separator =
        new QFrame(card);

    separator->setFrameShape(
        QFrame::HLine
        );

    separator->setFrameShadow(
        QFrame::Plain
        );

    separator->setStyleSheet(
        "QFrame {"
        "    color: #edf0f4;"
        "}"
        );

    layout->addWidget(separator);

    // Installation place
    {
        auto* row =
            new QWidget(card);

        row->setLayoutDirection(
            Qt::RightToLeft
            );

        auto* rowLayout =
            new QHBoxLayout(row);

        rowLayout->setContentsMargins(
            0, 0, 0, 0
            );

        auto* label =
            new QLabel(
                "محل نصب",
                row
                );

        label->setObjectName(
            "fieldTitle"
            );

        label->setAlignment(
            Qt::AlignLeft |
            Qt::AlignVCenter
            );

        m_quickInstallationPlaceLabel =
            new QLabel("-", row);

        m_quickInstallationPlaceLabel
            ->setObjectName("fieldValue");

        m_quickInstallationPlaceLabel
            ->setAlignment(
                Qt::AlignRight |
                Qt::AlignVCenter
                );

        m_quickInstallationPlaceLabel
            ->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Preferred
                );

        rowLayout->addWidget(label);
        rowLayout->addWidget(
            m_quickInstallationPlaceLabel,
            1
            );

        layout->addWidget(row);
    }

    // Installation date
    {
        auto* row =
            new QWidget(card);

        row->setLayoutDirection(
            Qt::RightToLeft
            );

        auto* rowLayout =
            new QHBoxLayout(row);

        rowLayout->setContentsMargins(
            0, 0, 0, 0
            );

        auto* label =
            new QLabel(
                "تاریخ نصب",
                row
                );

        label->setObjectName(
            "fieldTitle"
            );

        label->setAlignment(
            Qt::AlignLeft |
            Qt::AlignVCenter
            );

        m_quickInstallationDateLabel =
            new QLabel("-", row);

        m_quickInstallationDateLabel
            ->setObjectName("fieldValue");

        m_quickInstallationDateLabel
            ->setAlignment(
                Qt::AlignLeft |
                Qt::AlignVCenter
                );

        m_quickInstallationDateLabel
            ->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Preferred
                );

        rowLayout->addWidget(label);
        rowLayout->addWidget(
            m_quickInstallationDateLabel,
            1
            );

        layout->addWidget(row);
    }

    // Status
    {
        auto* row =
            new QWidget(card);

        row->setLayoutDirection(
            Qt::RightToLeft
            );

        auto* rowLayout =
            new QHBoxLayout(row);

        rowLayout->setContentsMargins(
            0, 0, 0, 0
            );

        auto* label =
            new QLabel(
                "وضعیت",
                row
                );

        label->setObjectName(
            "fieldTitle"
            );

        label->setAlignment(
            Qt::AlignLeft |
            Qt::AlignVCenter
            );

        m_quickStatusLabel =
            new QLabel(
                "● فعال",
                row
                );

        m_quickStatusLabel
            ->setObjectName(
                "statusLabel"
                );

        m_quickStatusLabel
            ->setAlignment(
                Qt::AlignRight |
                Qt::AlignVCenter
                );

        m_quickStatusLabel
            ->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Preferred
                );

        rowLayout->addWidget(label);
        rowLayout->addWidget(
            m_quickStatusLabel,
            1
            );

        layout->addWidget(row);
    }

    return card;
}

QLabel* AssetOverviewWidget::createFieldLabel(
    const QString& title,
    QWidget* parent
    )
{
    auto* label = new QLabel(title, parent);

    label->setObjectName("fieldTitle");
    label->setLayoutDirection(Qt::RightToLeft);
    label->setAlignment(Qt::AlignRight);

    return label;
}

void AssetOverviewWidget::setAsset(const Asset& asset)
{
    const AssetInfo& info = asset.info();
    m_assetId = asset.id();

    m_assetNameLabel->setText(
        info.name
        );

    m_assetTypeLabel->setText(
        info.assetType
        );

    m_serialNumberLabel->setText(
        info.serialNumber.isEmpty()
            ? "-"
            : info.serialNumber
        );
    m_serialNumberLabel->setAlignment(Qt::AlignLeft);

    m_manufacturerLabel->setText(
        info.manufacturer.isEmpty()
            ? "-"
            : info.manufacturer
        );
    m_manufacturerLabel->setAlignment(Qt::AlignLeft);

    m_modelLabel->setText(
        info.model.isEmpty()
            ? "-"
            : info.model
        );
    m_modelLabel->setAlignment(Qt::AlignLeft);

    m_installationDateLabel->setText(
        info.installationDate.isValid()
            ? toPersianDate(info.installationDate)
            : "-"
        );
    m_installationDateLabel->setAlignment(Qt::AlignLeft);

    m_installationPlaceLabel->setText(
        info.installationPlace.isEmpty()
            ? "-"
            : info.installationPlace
        );

    m_descriptionLabel->setText(
        info.description.isEmpty()
            ? "-"
            : info.description
        );

    /*
     * Quick information
     */

    m_quickInstallationPlaceLabel->setText(
        info.installationPlace.isEmpty()
            ? "-"
            : info.installationPlace
        );

    m_quickInstallationDateLabel->setText(
        info.installationDate.isValid()
            ? toPersianDate(info.installationDate)
            : "-"
        );

    /*
     * Image
     */

    if (!info.imagePath.isEmpty() &&
        QFileInfo::exists(info.imagePath))
    {
        QPixmap pixmap(info.imagePath);

        if (!pixmap.isNull())
        {
            m_assetImageLabel->setPixmap(
                pixmap.scaled(
                    m_assetImageLabel->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    )
                );

            return;
        }
    }

    m_assetImageLabel->setPixmap(QPixmap());
    m_assetImageLabel->setText("تصویر تجهیز");
}

QWidget* AssetOverviewWidget::createInfoRow(
    const QString& title,
    QLabel*& valueLabel,
    const QString& iconPath,
    QWidget* parent
    )
{
    auto* row = new QWidget(parent);
    row->setLayoutDirection(Qt::RightToLeft);

    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);

    auto* icon = new QLabel(row);
    icon->setFixedSize(18, 18);
    icon->setAlignment(Qt::AlignCenter);
    icon->setPixmap(QIcon(iconPath).pixmap(16, 16));

    auto* titleLabel = new QLabel(title, row);
    titleLabel->setObjectName("fieldTitle");
    titleLabel->setAlignment(Qt::AlignLeft);

    valueLabel = new QLabel("-", row);
    valueLabel->setObjectName("fieldValue");
    valueLabel->setTextInteractionFlags(
        Qt::TextSelectableByMouse
        );
    valueLabel->setAlignment(Qt::AlignRight);

    layout->addWidget(icon);
    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);

    return row;
}
QUuid AssetOverviewWidget::assetId() const
{
    return m_assetId;
}