#include "assetsection.h"

#include "asset.h"
#include "asseteditdialog.h"
#include "assetmanager.h"
#include "assettreewidget.h"
#include "assetwidget.h"
#include "persiandigits.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QFrame>
#include <QSizePolicy>
#include <QScrollArea>
#include <QDate>
#include <qdebug.h>
#include <qheaderview.h>

AssetSection::AssetSection(
    AssetManager& assetManager,
    QWidget* parent
    )
    : QWidget(parent),
    m_assetManager(assetManager)
{
    setupUI();
    rebuildTree();
}

void AssetSection::setupUI()
{
    setLayoutDirection(
        Qt::RightToLeft
        );

    auto* mainLayout =
        new QHBoxLayout(this);

    mainLayout->setContentsMargins(
        20, 20, 20, 20
        );

    mainLayout->setSpacing(16);

    // ========================================================
    // Main asset panel
    // ========================================================

    auto* assetPanel =
        new QFrame(this);

    assetPanel->setStyleSheet(
        "QFrame#assetPanel {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 12px;"
        "}"
        );

    assetPanel->setObjectName(
        "assetPanel"
        );

    auto* assetLayout =
        new QVBoxLayout(assetPanel);

    assetLayout->setContentsMargins(
        14, 14, 14, 14
        );

    assetLayout->setSpacing(12);

    // ========================================================
    // Header
    // ========================================================

    auto* header =
        new QWidget(assetPanel);

    auto* headerLayout =
        new QHBoxLayout(header);

    headerLayout->setContentsMargins(
        0, 0, 0, 0
        );

    headerLayout->setSpacing(10);

    auto* titleLayout =
        new QVBoxLayout();

    titleLayout->setContentsMargins(
        0, 0, 0, 0
        );

    titleLayout->setSpacing(2);

    auto* title =
        new QLabel(
            "تجهیزات",
            header
            );

    title->setStyleSheet(
        "font-size: 22px;"
        "font-weight: 700;"
        "color: #172554;"
        "border: none;"
        );

    auto* description =
        new QLabel(
            "مدیریت و مشاهده تمامی تجهیزات",
            header
            );

    description->setStyleSheet(
        "font-size: 12px;"
        "color: #64748b;"
        "border: none;"
        );

    titleLayout->addWidget(title);
    titleLayout->addWidget(description);

    headerLayout->addLayout(
        titleLayout,
        1
        );

    // --------------------------------------------------------
    // Add button
    // --------------------------------------------------------

    m_addButton =
        new QPushButton(
            QIcon(
                ":/qlementine/icons/16/action/plus.svg"
                ),
            "افزودن تجهیزات",
            header
            );

    m_addButton->setMinimumHeight(
        40
        );

    m_addButton->setLayoutDirection(
        Qt::RightToLeft
        );

    m_addButton->setCursor(
        Qt::PointingHandCursor
        );

    connect(
        m_addButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            AssetInfo info;

            AssetEditDialog dialog(
                info,
                this
                );

            dialog.setWindowTitle("افزودن تجهیز");

            if (dialog.exec() != QDialog::Accepted)
                return;

            emit addAssetRequested(
                dialog.assetInfo()
                );
        }
        );

    headerLayout->addWidget(
        m_addButton
        );

    assetLayout->addWidget(
        header
        );

    // ========================================================
    // Search
    // ========================================================

    auto* searchLayout =
        new QHBoxLayout();

    searchLayout->setContentsMargins(
        0, 0, 0, 0
        );

    searchLayout->setSpacing(8);

    m_searchEdit =
        new QLineEdit(assetPanel);

    m_searchEdit->setPlaceholderText(
        "جستجو در نام یا شماره سریال تجهیزات..."
        );

    m_searchEdit->setLayoutDirection(
        Qt::RightToLeft
        );

    m_searchEdit->setMinimumHeight(
        38
        );

    searchLayout->addWidget(
        m_searchEdit,
        1
        );

    assetLayout->addLayout(
        searchLayout
        );

    connect(
        m_searchEdit,
        &QLineEdit::textChanged,
        this,
        &AssetSection::filterTree
        );

    // ========================================================
    // Tree
    // ========================================================

    m_assetTree =
        new AssetTreeWidget(assetPanel);

    m_assetTree->setColumnCount(1);
    m_assetTree->setHeaderHidden(true);
    m_assetTree->setRootIsDecorated(true);
    m_assetTree->setIndentation(22);
    m_assetTree->setAnimated(false);
    m_assetTree->setUniformRowHeights(false);
    m_assetTree->setFrameShape(QFrame::NoFrame);

    connect(
        m_assetTree,
        &QTreeWidget::itemClicked,
        this,
        [](QTreeWidgetItem* item, int column)
        {
            qDebug() << "AssetTree itemClicked:" << item << column;
        }
        );

    m_assetTree->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    m_assetTree->setVerticalScrollMode(
        QAbstractItemView::ScrollPerPixel
        );

    m_assetTree->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    m_assetTree->header()->setStretchLastSection(true);

    m_assetTree->header()->setSectionResizeMode(
        0,
        QHeaderView::Stretch
        );

    m_assetTree->setStyleSheet(
        "QTreeWidget {"
        "    background: transparent;"
        "    border: none;"
        "    outline: none;"
        "}"
        ""
        "QTreeWidget::item {"
        "    border: none;"
        "    padding: 0px;"
        "}"
        ""
        "QTreeWidget::branch {"
        "    background: transparent;"
        "}"
        );

    assetLayout->addWidget(
        m_assetTree,
        1
        );

    // ========================================================
    // Info panel
    // ========================================================

    m_infoPanel =
        new QFrame(this);

    m_infoPanel->setMinimumWidth(
        300
        );

    m_infoPanel->setMaximumWidth(
        360
        );

    m_infoPanel->setStyleSheet(
        "QFrame#assetInfoPanel {"
        "    background: #ffffff;"
        "    border: 1px solid #e2e8f0;"
        "    border-radius: 12px;"
        "}"
        );

    m_infoPanel->setObjectName(
        "assetInfoPanel"
        );

    m_infoLayout =
        new QVBoxLayout(m_infoPanel);

    m_infoLayout->setContentsMargins(
        14, 14, 14, 14
        );

    m_infoLayout->setSpacing(
        6
        );

    // --------------------------------------------------------
    // Info header
    // --------------------------------------------------------

    auto* infoHeader = new QHBoxLayout();

    infoHeader->setContentsMargins(
        0, 0, 0, 0
        );

    infoHeader->setSpacing(
        10
        );

    m_infoImageLabel =
        new QLabel(m_infoPanel);

    m_infoImageLabel->setFixedSize(
        210, 210
        );

    m_infoImageLabel->setAlignment(
        Qt::AlignCenter
        );

    m_infoImageLabel->setStyleSheet(
        "background: #f1f5f9;"
        "border-radius: 10px;"
        "border: none;"
        );

    // --------------------------------------------------------
    // Header text
    // --------------------------------------------------------

    auto* infoTextWidget =
        new QWidget(m_infoPanel);

    auto* infoText =
        new QVBoxLayout(infoTextWidget);

    infoText->setContentsMargins(
        0, 0, 0, 0
        );

    infoText->setSpacing(
        4
        );

    m_infoNameLabel =
        new QLabel(
            "تجهیز انتخاب نشده",
            infoTextWidget
            );

    m_infoNameLabel->setWordWrap(true);

    m_infoNameLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
        );

    m_infoNameLabel->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 700;"
        "color: #172554;"
        "border: none;"
        );

    m_infoTypeLabel =
        new QLabel(
            "",
            infoTextWidget
            );

    m_infoTypeLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
        );

    m_infoTypeLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #64748b;"
        "border: none;"
        );

    // Keep text at the top
    infoText->addWidget(
        m_infoNameLabel
        );

    infoText->addWidget(
        m_infoTypeLabel
        );

    infoText->addStretch();

    infoHeader->addWidget(
        m_infoImageLabel,
        0,
        Qt::AlignTop
        );

    infoHeader->addWidget(
        infoTextWidget,
        1,
        Qt::AlignTop
        );

    m_infoLayout->addLayout(
        infoHeader,
        0
        );

    // --------------------------------------------------------
    // Info separator
    // --------------------------------------------------------

    auto* separator =
        new QFrame(m_infoPanel);

    separator->setFrameShape(
        QFrame::HLine
        );

    separator->setFrameShadow(
        QFrame::Plain
        );

    separator->setStyleSheet(
        "color: #e2e8f0;"
        );

    m_infoLayout->addWidget(
        separator
        );
    // Info items will be inserted here.

    m_infoItemsLayout = new QVBoxLayout();

    m_infoItemsLayout->setContentsMargins(
        0, 0, 0, 0
        );

    m_infoItemsLayout->setSpacing(
        2
        );

    m_infoLayout->addLayout(
        m_infoItemsLayout
        );

    //m_infoLayout->addStretch();

    // ========================================================
    // Main layout
    // ========================================================

    mainLayout->addWidget(
        assetPanel,
        1
        );

    mainLayout->addWidget(
        m_infoPanel
        );

    mainLayout->setStretch(
        0,
        7
        );

    mainLayout->setStretch(
        1,
        3
        );
}

void AssetSection::rebuildTree()
{
    qDebug() << "AssetSection:" << this;
    qDebug() << "m_assetTree:" << m_assetTree;
    m_assetTree->clear();
    qDebug() << "assset tree clear";
    m_selectedAssetId = QUuid();
    qDebug() << "Selected asset quid set";
    for (const Asset& asset :
         m_assetManager.assets())
    {
        createAssetItem(asset);
        qDebug() << "created asset items";
    }

    if (m_assetTree->topLevelItemCount() > 0)
    {
        qDebug() << "Selcting first asset";
        auto* first =
            m_assetTree->topLevelItem(0);

        m_assetTree->setCurrentItem(
            first
            );

        const QUuid id =
            first->data(
                     0,
                     Qt::UserRole
                     ).toUuid();

        selectAsset(id);
    }
    else
    {
        // No assets remain
        m_infoNameLabel->setText(
            "تجهیز انتخاب نشده"
            );

        m_infoTypeLabel->clear();
        m_infoImageLabel->clear();

        while (m_infoItemsLayout->count() > 0)
        {
            QLayoutItem* item =
                m_infoItemsLayout->takeAt(0);

            if (!item)
                break;

            if (item->widget())
                item->widget()->deleteLater();

            delete item;
        }
    }
}
QTreeWidgetItem*
AssetSection::createAssetItem(
    const Asset& asset
    )
{
    auto* item =
        new QTreeWidgetItem();

    item->setData(
        0,
        Qt::UserRole,
        asset.id()
        );

    // IMPORTANT:
    // The item must be inserted into the tree
    // before setItemWidget() is called.
    m_assetTree->addTopLevelItem(item);

    item->setSizeHint(
        0,
        QSize(0, 84)
        );

    auto* widget =
        new AssetWidget(
            asset.info().name,
            asset.info().serialNumber,
            asset.info().imagePath,
            asset.info().installationDate,
            asset.info().installationPlace,
            "فعال",
            "#16a34a",
            asset.id(),
            true,
            m_assetTree
            );

    m_assetTree->setItemWidget(
        item,
        0,
        widget
        );

    connect(
        widget,
        &AssetWidget::clicked,
        this,
        [this, item]()
        {
            qDebug() << "AssetSection TOP LEVEL CLICKED";

            m_assetTree->setCurrentItem(item);

            const QUuid assetId =
                item->data(
                        0,
                        Qt::UserRole
                        ).toUuid();

            qDebug() << "AssetSection selected ID:" << assetId;

            if (!assetId.isNull())
                selectAsset(assetId);
        }
        );

    connect(
        widget,
        &AssetWidget::editRequested,
        this,
        [this](const QUuid& assetId)
        {
            emit editAssetRequested(assetId);
        }
        );

    connect(
        widget,
        &AssetWidget::deleteRequested,
        this,
        [this](const QUuid& assetId)
        {
            emit deleteAssetRequested(assetId);
        }
        );

    connect(
        widget,
        &AssetWidget::openRequested,
        this,
        [this](const QUuid& assetId)
        {
            emit openAssetRequested(assetId);
        }
        );

    // --------------------------------------------------------
    // Sub-assets
    // --------------------------------------------------------

    for (const auto& subAsset :
         asset.subAssets())
    {
        auto* child =
            new QTreeWidgetItem(item);

        child->setSizeHint(
            0,
            QSize(0, 84)
            );

        child->setData(
            0,
            Qt::UserRole,
            asset.id()
            );

        auto* childWidget =
            new AssetWidget(
                subAsset.info().name,
                subAsset.info().serialNumber,
                subAsset.info().imagePath,
                subAsset.info().installationDate,
                subAsset.info().installationPlace,
                "فعال",
                "#16a34a",
                asset.id(),
                false,
                m_assetTree
                );

        connect(
            childWidget,
            &AssetWidget::clicked,
            this,
            [this, child]()
            {
                m_assetTree->setCurrentItem(child);

                const QUuid assetId =
                    child->data(
                             0,
                             Qt::UserRole
                             ).toUuid();

                if (!assetId.isNull())
                    selectAsset(assetId);
            }
            );

        m_assetTree->setItemWidget(
            child,
            0,
            childWidget
            );
    }

    return item;
}

void AssetSection::filterTree(
    const QString& text
    )
{
    const QString query =
        text.trimmed();

    for (int i = 0;
         i < m_assetTree->topLevelItemCount();
         ++i)
    {
        auto* item =
            m_assetTree->topLevelItem(i);

        const bool visible =
            filterItem(item, query);

        item->setHidden(
            !visible
            );

        if (!query.isEmpty() && visible)
        {
            item->setExpanded(true);
        }
    }
}


bool AssetSection::filterItem(
    QTreeWidgetItem* item,
    const QString& text
    )
{
    if (!item)
        return false;

    if (text.isEmpty())
    {
        for (int i = 0;
             i < item->childCount();
             ++i)
        {
            item->child(i)->setHidden(
                false
                );
        }

        return true;
    }

    const QUuid assetId =
        item->data(
                0,
                Qt::UserRole
                ).toUuid();

    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (!asset)
        return false;

    bool selfMatches = false;

    // --------------------------------------------------------
    // Parent asset
    // --------------------------------------------------------

    selfMatches =
        asset->info().name.contains(
            text,
            Qt::CaseInsensitive
            )
        ||
        asset->info().serialNumber.contains(
            text,
            Qt::CaseInsensitive
            );

    // --------------------------------------------------------
    // Children
    // --------------------------------------------------------

    bool childMatches = false;

    for (int i = 0;
         i < item->childCount();
         ++i)
    {
        auto* child =
            item->child(i);

        const bool matches =
            filterItem(
                child,
                text
                );

        child->setHidden(
            !matches
            );

        childMatches |= matches;
    }

    return selfMatches || childMatches;
}

void AssetSection::selectAsset(
    const QUuid& assetId
    )
{
    Asset* asset =
        m_assetManager.getAsset(
            assetId
            );

    if (!asset)
        return;

    m_selectedAssetId =
        assetId;

    updateInfoPanel(
        *asset
        );

    emit assetSelected(
        assetId
        );
}

QWidget* AssetSection::createInfoItem(
    const QString& iconPath,
    const QString& label,
    const QString& value
    )
{
    auto* widget = new QWidget(this);

    auto* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 4, 0, 4);
    layout->setSpacing(8);

    auto* icon = new QLabel(widget);
    icon->setFixedSize(18, 18);
    icon->setAlignment(Qt::AlignCenter);
    icon->setPixmap(
        QIcon(iconPath).pixmap(16, 16)
        );

    auto* labelWidget = new QLabel(label, widget);
    labelWidget->setStyleSheet(
        "font-size: 11px;"
        "font-weight: 600;"
        "color: #64748b;"
        "border: none;"
        );

    auto* valueWidget = new QLabel(value, widget);
    valueWidget->setStyleSheet(
        "font-size: 12px;"
        "color: #172554;"
        "border: none;"
        );
    valueWidget->setTextInteractionFlags(
        Qt::TextSelectableByMouse
        );

    layout->addWidget(icon);
    layout->addWidget(labelWidget);
    layout->addStretch();
    layout->addWidget(valueWidget);


    return widget;
}
QWidget* AssetSection::createDescriptionItem(
    const QString& iconPath,
    const QString& label,
    const QString& value
    )
{
    auto* widget = new QWidget(this);

    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 8, 0, 4);
    layout->setSpacing(6);

    auto* header = new QWidget(widget);

    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    auto* icon = new QLabel(header);
    icon->setFixedSize(18, 18);
    icon->setAlignment(Qt::AlignCenter);
    icon->setPixmap(
        QIcon(iconPath).pixmap(16, 16)
        );

    auto* labelWidget = new QLabel(label, header);
    labelWidget->setStyleSheet(
        "font-size: 11px;"
        "font-weight: 600;"
        "color: #64748b;"
        "border: none;"
        );

    headerLayout->addWidget(icon);
    headerLayout->addWidget(labelWidget);
    headerLayout->addStretch();

    auto* valueWidget = new QLabel(value, widget);
    valueWidget->setWordWrap(true);
    valueWidget->setStyleSheet(
        "font-size: 12px;"
        "color: #172554;"
        "border: none;"
        );

    layout->addWidget(header);
    layout->addWidget(valueWidget);

    return widget;
}

void AssetSection::updateInfoPanel(
    const Asset& asset
    )
{
    const AssetInfo& info = asset.info();

    m_infoNameLabel->setText(
        info.name
        );

    m_infoTypeLabel->setText(
        info.assetType
        );

    // --------------------------------------------------------
    // Image
    // --------------------------------------------------------

    QPixmap pixmap;

    if (!info.imagePath.isEmpty())
        pixmap.load(info.imagePath);

    if (!pixmap.isNull())
    {
        m_infoImageLabel->setPixmap(
            pixmap.scaled(
                200,
                200,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    }
    else
    {
        m_infoImageLabel->setPixmap(
            QIcon(
                ":/qlementine/icons/16/instrument/machine.svg"
                ).pixmap(200, 200)
            );
    }

    // --------------------------------------------------------
    // Remove previous info items
    // --------------------------------------------------------

    while (m_infoItemsLayout->count() > 0)
    {
        QLayoutItem* item =
            m_infoItemsLayout->takeAt(0);

        if (!item)
            break;

        if (item->widget())
            item->widget()->deleteLater();

        delete item;
    }

    // --------------------------------------------------------
    // Information
    // --------------------------------------------------------

    m_infoItemsLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/calendar.svg",
            "تاریخ نصب",
            info.installationDate.isValid()
                ? toPersianDigits(
                      toPersianDate(info.installationDate)
                      )
                : "-"
            )
        );

    m_infoItemsLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/location.svg",
            "محل نصب",
            toPersianDigits(
                info.installationPlace
                )
            )
        );

    m_infoItemsLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/user.svg",
            "سازنده",
            toPersianDigits(
                info.manufacturer
                )
            )
        );

    m_infoItemsLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/tag.svg",
            "مدل",
            info.model
            )
        );

    m_infoItemsLayout->addWidget(
        createInfoItem(
            ":/qlementine/icons/custome/serial-number.svg",
            "شماره سریال",
            info.serialNumber
            )
        );

    // --------------------------------------------------------
    // Description
    // --------------------------------------------------------

    m_infoItemsLayout->addWidget(
        createDescriptionItem(
            ":/qlementine/icons/custome/note.svg",
            "توضیحات",
            toPersianDigits(
                info.description
                )
            )
        );
}

void AssetSection::refreshAssets()
{
    rebuildTree();
}