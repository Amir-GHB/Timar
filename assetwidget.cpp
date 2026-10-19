#include "assetwidget.h"
#include "persiandigits.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <qdebug.h>
#include <qmenu.h>
#include <qtoolbutton.h>

AssetWidget::AssetWidget(
    const QString& name,
    const QString& serialNumber,
    const QString& imagePath,
    const QDate& installationDate,
    const QString& installationPlace,
    const QString& statusText,
    const QString& statusColor,
    const QUuid& assetId,
    bool isAsset,
    QWidget* parent
    )
    : QFrame(parent),
    m_name(name),
    m_serialNumber(serialNumber),
    m_imagePath(imagePath),
    m_installationDate(installationDate),
    m_installationPlace(installationPlace),
    m_statusText(statusText),
    m_statusColor(statusColor),
    m_assetId(assetId),
    m_isAsset(isAsset)
{
    setObjectName("assetWidget");

    setMinimumHeight(76);

    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
        );

    setCursor(Qt::PointingHandCursor);

    setStyleSheet(
        "#assetWidget {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 10px;"
        "}"
        ""
        "#assetWidget:hover {"
        "    border: 1px solid #93c5fd;"
        "    background: #f8fbff;"
        "}"
        );

    setupUI();
}

void AssetWidget::setupUI()
{
    auto* layout =
        new QHBoxLayout(this);

    layout->setContentsMargins(
        12, 8, 12, 8
        );

    layout->setSpacing(12);

    // ========================================================
    // Asset image
    // ========================================================

    m_imageLabel =
        new QLabel(this);

    m_imageLabel->setFixedSize(
        56, 56
        );

    m_imageLabel->setAlignment(
        Qt::AlignCenter
        );

    m_imageLabel->setStyleSheet(
        "background: #f1f5f9;"
        "border-radius: 9px;"
        "border: none;"
        );

    QPixmap pixmap;

    if (!m_imagePath.isEmpty())
    {
        pixmap.load(m_imagePath);
    }

    if (!pixmap.isNull())
    {
        m_imageLabel->setPixmap(
            pixmap.scaled(
                44,
                44,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    }
    else
    {
        // Temporary default equipment icon.
        // We can change this to your preferred Qlementine icon.
        m_imageLabel->setPixmap(
            QIcon(
                ":/qlementine/icons/custome/one-gear.svg"
                ).pixmap(32, 32)
            );

        m_imageLabel->setStyleSheet(
            "background: #f1f5f9;"
            "border-radius: 9px;"
            "font-size: 24px;"
            "color: #64748b;"
            "border: none;"
            );
    }

    // ========================================================
    // Status
    // ========================================================

    m_statusLabel =
        new QLabel(
            m_statusText,
            this
            );

    m_statusLabel->setAlignment(
        Qt::AlignCenter
        );

    m_statusLabel->setMinimumWidth(
        55
        );

    m_statusLabel->setFixedHeight(22);
    m_statusLabel->setFixedWidth(55);

    m_statusLabel->setStyleSheet(
        QString(
            "QLabel {"
            "    background: %1;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 10px;"
            "    padding: 4px 10px;"
            "    font-size: 10px;"
            "}"
            ).arg(m_statusColor)
        );

    // ========================================================
    // Name + serial
    // ========================================================

    auto* textWidget =
        new QWidget(this);

    auto* textLayout =
        new QVBoxLayout(textWidget);

    textLayout->setContentsMargins(
        0, 10, 0, 0
        );

    textLayout->setSpacing(3);

    m_nameLabel =
        new QLabel(
            m_name,
            textWidget
            );

    m_nameLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 600;"
        "color: #172554;"
        "border: none;"
        );

    m_serialLabel =
        new QLabel(
            m_serialNumber,
            textWidget
            );

    m_serialLabel->setStyleSheet(
        "font-size: 11px;"
        "color: #64748b;"
        "border: none;"
        );

    m_serialLabel->setAlignment(Qt::AlignRight);

    textLayout->addWidget(
        m_nameLabel
        );

    textLayout->addWidget(
        m_serialLabel
        );

    // ========================================================
    // Installation date + place
    // ========================================================

    auto* installationWidget =
        new QWidget(this);

    auto* installationLayout =
        new QVBoxLayout(installationWidget);

    installationLayout->setContentsMargins(
        0, 0, 0, 0
        );

    installationLayout->setSpacing(0);

    // --------------------------------------------------------
    // Installation date row
    // --------------------------------------------------------

    auto* dateRow =
        new QWidget(installationWidget);

    auto* dateLayout =
        new QHBoxLayout(dateRow);

    dateLayout->setContentsMargins(
        0, 15, 0, 0
        );

    dateLayout->setSpacing(5);

    auto* dateIcon =
        new QLabel(dateRow);

    dateIcon->setFixedSize(16, 16);
    dateIcon->setAlignment(Qt::AlignCenter);

    dateIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/custome/calendar.svg"
            ).pixmap(14, 14)
        );

    dateIcon->setStyleSheet(
        "border: none;"
        );

    m_installationDateLabel =
        new QLabel(
            toPersianDigits(
                m_installationDate.isValid()
                    ? toPersianDate(m_installationDate)
                    : "-"
                ),
            dateRow
            );

    m_installationDateLabel->setStyleSheet(
        "font-size: 11px;"
        "color: #475569;"
        "border: none;"
        );

    m_installationDateLabel->setAlignment(
        Qt::AlignRight
        );

    dateLayout->addWidget(dateIcon);

    dateLayout->addWidget(
        m_installationDateLabel
        );

    // --------------------------------------------------------
    // Installation place row
    // --------------------------------------------------------

    auto* placeRow =
        new QWidget(installationWidget);

    auto* placeLayout =
        new QHBoxLayout(placeRow);

    placeLayout->setContentsMargins(
        0, 0, 0, 0
        );

    placeLayout->setSpacing(5);

    auto* placeIcon =
        new QLabel(placeRow);

    placeIcon->setFixedSize(16, 16);
    placeIcon->setAlignment(Qt::AlignCenter);

    placeIcon->setPixmap(
        QIcon(
            ":/qlementine/icons/16/misc/location.svg"
            ).pixmap(14, 14)
        );

    placeIcon->setStyleSheet(
        "border: none;"
        );

    m_installationPlaceLabel =
        new QLabel(
            m_installationPlace,
            placeRow
            );

    m_installationPlaceLabel->setStyleSheet(
        "font-size: 11px;"
        "color: #64748b;"
        "border: none;"
        );

    placeLayout->addWidget(placeIcon);

    placeLayout->addWidget(
        m_installationPlaceLabel
        );

    // --------------------------------------------------------
    // Add rows
    // --------------------------------------------------------

    installationLayout->addWidget(
        dateRow
        );

    installationLayout->addWidget(
        placeRow
        );

    // ========================================================
    // Popup menu + arrow
    //
    // Only top-level Assets get these controls.
    // SubAssets do not create them.
    // ========================================================

    auto* actionLayout = new QVBoxLayout();
    actionLayout->setSpacing(4);
    actionLayout->setContentsMargins(0, 0, 0, 0);

    if (m_isAsset)
    {


        // ========================================================
        // Delete button
        // ========================================================

        auto* deleteButton = new QToolButton(this);

        deleteButton->setFixedSize(26, 26);
        deleteButton->setCursor(Qt::PointingHandCursor);

        deleteButton->setIcon(
            QIcon(":/qlementine/icons/custome/trash-red.svg")
            );

        deleteButton->setIconSize(QSize(14, 14));

        deleteButton->setStyleSheet(
            "QToolButton {"
            "    background: #fee2e2;"
            "    border: none;"
            "    border-radius: 13px;"
            "    padding: 0px;"
            "}"
            "QToolButton:hover {"
            "    background: #fecaca;"
            "}"
            "QToolButton:pressed {"
            "    background: #fca5a5;"
            "}"
            );

        connect(
            deleteButton,
            &QToolButton::clicked,
            this,
            [this]()
            {
                qDebug() << "AssetWidget DELETE:" << m_assetId;
                emit deleteRequested(m_assetId);
            }
            );


        // ========================================================
        // Edit button
        // ========================================================

        auto* editButton = new QToolButton(this);

        editButton->setFixedSize(26, 26);
        editButton->setCursor(Qt::PointingHandCursor);

        editButton->setIcon(
            QIcon(":/qlementine/icons/custome/pen-green.svg")
            );

        editButton->setIconSize(QSize(14, 14));

        editButton->setStyleSheet(
            "QToolButton {"
            "    background: #dcfce7;"
            "    border: none;"
            "    border-radius: 13px;"
            "    padding: 0px;"
            "}"
            "QToolButton:hover {"
            "    background: #bbf7d0;"
            "}"
            "QToolButton:pressed {"
            "    background: #86efac;"
            "}"
            );

        connect(
            editButton,
            &QToolButton::clicked,
            this,
            [this]()
            {
                qDebug() << "AssetWidget EDIT:" << m_assetId;
                emit editRequested(m_assetId);
            }
            );


        // ========================================================
        // Open button
        // ========================================================

        auto* openButton = new QToolButton(this);

        openButton->setFixedSize(26, 26);
        openButton->setCursor(Qt::PointingHandCursor);

        openButton->setIcon(
            QIcon(":/qlementine/icons/custome/arrow-left-blue.svg")
            );

        openButton->setIconSize(QSize(14, 14));

        openButton->setStyleSheet(
            "QToolButton {"
            "    background: #dbeafe;"
            "    border: none;"
            "    border-radius: 13px;"
            "    padding: 0px;"
            "}"
            "QToolButton:hover {"
            "    background: #bfdbfe;"
            "}"
            "QToolButton:pressed {"
            "    background: #93c5fd;"
            "}"
            );

        connect(
            openButton,
            &QToolButton::clicked,
            this,
            [this]()
            {
                qDebug() << "AssetWidget OPEN:" << m_assetId;
                emit openRequested(m_assetId);
            }
            );


        // ========================================================
        // Add buttons
        // ========================================================

        actionLayout->addWidget(
            deleteButton,
            0,
            Qt::AlignHCenter
            );

        actionLayout->addWidget(
            editButton,
            0,
            Qt::AlignHCenter
            );

        actionLayout->addWidget(
            openButton,
            0,
            Qt::AlignHCenter
            );
    }

    // ========================================================
    // RTL
    // ========================================================

    layout->setDirection(
        QBoxLayout::LeftToRight
        );

    textWidget->setFixedWidth(220);

    installationWidget->setFixedWidth(
        180
        );

    m_statusLabel->setFixedWidth(
        55
        );

    m_nameLabel->setWordWrap(false);

    m_nameLabel->setMaximumWidth(
        220
        );

    layout->addWidget(
        m_imageLabel
        );

    layout->addWidget(
        textWidget
        );

    layout->addWidget(
        m_statusLabel
        );

    layout->addSpacing(60);

    layout->addWidget(
        installationWidget
        );

    layout->addLayout(
        actionLayout
        );
}

void AssetWidget::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "AssetWidget mousePress:" << event->button();

    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }

    QFrame::mousePressEvent(event);
}