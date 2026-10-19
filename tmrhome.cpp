#include "tmrhome.h"
#include "asseteditdialog.h"
#include "assetoverviewwidget.h"
#include "assetsection.h"
#include "assettree.h"
#include "maintenanceeventdialog.h"
#include "maintenanceeventwidget.h"
#include "maintenancescheduledialog.h"
#include "maintenanceschedulewidget.h"
#include "navigationpanel.h"
#include "assetmanager.h"
#include "persiandigits.h"
#include "reportssection.h"
#include "settingswidget.h"
#include "storagesection.h"
#include "subassetswidget.h"
#include "timelinewidget.h"


#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QTreeWidget>
#include <QSizePolicy>
#include <QWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QIcon>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QTabWidget>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QPixmap>
#include <qdebug.h>
#include <qdialog.h>
#include <QMessageBox>
#include <qscrollarea.h>

Tmrhome::Tmrhome(QWidget *parent)
    : QMainWindow(parent),
    m_assetManager(),
    m_maintenanceManager(
        m_assetManager,
        this
        ),
    m_storageManager(m_assetManager, this),
    m_dataManager(
        m_assetManager,
        m_maintenanceManager,
        m_storageManager,
        this
        )
{
    if (!m_dataManager.load())
    {
        createDemoAssets();
        //createDemoEvents();

        m_dataManager.save();
    }
    setupUI();
}

Tmrhome::~Tmrhome() = default;

void Tmrhome::setupUI()
{
    setWindowTitle(QStringLiteral("سامانه مدیریت نگهداری"));
    resize(1200, 750);
    setMinimumSize(1200, 750);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);


    auto* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Main content splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    m_mainSplitter->setHandleWidth(4);
    m_mainSplitter->setChildrenCollapsible(false);

    // Create the panels without assigning the splitter as parent.
    treePanel = createTreePanel(nullptr);

    createMaintContents(nullptr);

    // Add them to the splitter.
    m_mainSplitter->addWidget(treePanel);
    m_mainSplitter->addWidget(m_mainContentStack);

    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setSizes({250, 800});

    // Navigation panel
    m_navigationPanel = createNavPanel(centralWidget);

    // Main content on the left, navigation on the right
    mainLayout->addWidget(m_mainSplitter, 1);
    mainLayout->addWidget(m_navigationPanel);

    connect(
        assetTree,
        &QTreeWidget::currentItemChanged,
        this,
        [this](QTreeWidgetItem* current, QTreeWidgetItem*) {
            if (!current)
                return;

            const QUuid assetId(
                current->data(0, Qt::UserRole).toString());

            Asset* asset = m_assetManager.getAsset(assetId);

            if (asset)
                updateAssetContent(*asset);
        });

    setupAssetTree();

    connect(&m_maintenanceManager,
            &MaintenanceManager::eventsUpdated,
            this,
            [this]() { refreshApplicationData(); });

    connect(&m_maintenanceManager,
            &MaintenanceManager::schedulesUpdated,
            this,
            [this]() { refreshApplicationData(); });
}

NavigationPanel* Tmrhome::createNavPanel(QWidget* parent)
{
    auto* sidePanel = new NavigationPanel(parent);


    auto* buttonGroup = new QButtonGroup(sidePanel);
    buttonGroup->setExclusive(true);

    // Placeholder icon paths. Replace these with your actual resource paths.
    const QString overviewIcon = ":/qlementine/icons/16/navigation/home.svg";
    const QString equipmentIcon = ":/qlementine/icons/16/misc/tool.svg";
    const QString timelineIcon = ":/qlementine/icons/16/misc/clock.svg";
    const QString reportsIcon = ":/qlementine/icons/custome/design.svg";
    const QString storageIcon =  ":/qlementine/icons/custome/warehouse.svg";
    const QString settingsIcon = ":/qlementine/icons/16/navigation/settings.svg";

    auto createButton = [&](const QString& text,
                            const QString& iconPath,
                            const QString& objectName)
    {
        auto* button = new QPushButton(QIcon(iconPath), text, sidePanel);

        button->setObjectName(objectName);
        button->setProperty("navigationText", text);
        button->setMinimumHeight(45);
        button->setCheckable(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setLayoutDirection(Qt::RightToLeft);

        buttonGroup->addButton(button);
        sidePanel->addNavigationButton(button);

        return button;
    };

    // Navigation buttons
    auto* overviewButton = createButton(
        QStringLiteral("نمای کلی"),
        overviewIcon,
        "overviewButton");

    auto* equipmentButton = createButton(
        QStringLiteral("تجهیزات"),
        equipmentIcon,
        "equipmentButton");

    auto* timelineButton = createButton(
        QStringLiteral("تایم لاین"),
        timelineIcon,
        "timelineButton");

    auto* reportsButton = createButton(
        QStringLiteral("گزارش ها"),
        reportsIcon,
        "reportsButton");

    auto* storageButton = createButton(
        QStringLiteral("انبار قطعات"),
        storageIcon,
        "storageButton");

    auto* settingsButton = createButton(
        QStringLiteral("تنظیمات"),
        settingsIcon,
        "settingsButton");

    // Default selection
    overviewButton->setChecked(true);

    // Page navigation
    connect(
        overviewButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mainContentStack->setCurrentIndex(0);
            treePanel->show();
        }
        );

    connect(
        equipmentButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mainContentStack->setCurrentIndex(1);
            treePanel->hide();
        }
        );

    connect(
        timelineButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mainContentStack->setCurrentIndex(2);
            treePanel->hide();
        }
        );

    connect(
        reportsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mainContentStack->setCurrentIndex(3);
            treePanel->hide();
        }
        );

    connect(
        storageButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mainContentStack->setCurrentIndex(4);
            treePanel->hide();
        }
        );

    connect(
        settingsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mainContentStack->setCurrentIndex(5);
            treePanel->hide();
        }
        );

    connect(
        sidePanel,
        &NavigationPanel::expandedChanged,
        this,
        [overviewButton,
         equipmentButton,
         timelineButton,
         reportsButton,
         storageButton,
         settingsButton]
        (bool expanded)
        {
            const QList<QPushButton*> buttons =
                {
                    overviewButton,
                    equipmentButton,
                    timelineButton,
                    reportsButton,
                    storageButton,
                    settingsButton
                };

            for (auto* button : buttons)
            {
                if (expanded)
                {
                    button->setText(
                        button->property("navigationText").toString()
                        );
                }
                else
                {
                    button->setText(QString());
                }
            }
        }
        );
    return sidePanel;
}

QFrame* Tmrhome::createMaintContents(QWidget* parent)
{
    m_mainContentStack = new QStackedWidget(m_mainSplitter);
    m_mainContentStack->setMinimumWidth(400);

    auto assetPanel = createAssetContentPanel(m_mainContentStack);
    m_mainContentStack->addWidget(assetPanel);

    m_assetSection =
        new AssetSection(
            m_assetManager,
            m_mainContentStack
            );

    qDebug() << "Tmrhome: AFTER AssetSection";

    connect(
        m_assetSection,
        &AssetSection::addAssetRequested,
        this,
        [this](const AssetInfo& info)
        {
            Asset* asset =
                m_assetManager.addAsset(info);

            if (!asset)
                return;

            refreshApplicationData();
        }
        );

    connect(
        m_assetSection,
        &AssetSection::editAssetRequested,
        this,
        [this](const QUuid& assetId)
        {
            Asset* asset =
                m_assetManager.getAsset(assetId);

            if (!asset)
                return;

            AssetEditDialog dialog(
                asset->info(),
                this
                );

            if (dialog.exec() != QDialog::Accepted)
                return;

            if (!m_assetManager.updateAsset(
                    assetId,
                    dialog.assetInfo()))
            {
                return;
            }

            refreshApplicationData();
        }
        );

    connect(
        m_assetSection,
        &AssetSection::deleteAssetRequested,
        this,
        [this](const QUuid& assetId)
        {
            QMessageBox messageBox(this);

            messageBox.setWindowTitle(
                "حذف تجهیز"
                );

            messageBox.setText(
                "آیا از حذف این تجهیز اطمینان دارید؟"
                );

            messageBox.setInformativeText(
                "تمام اطلاعات مرتبط با این تجهیز نیز حذف خواهند شد."
                );

            messageBox.setIcon(
                QMessageBox::Warning
                );

            QPushButton* yesButton =
                messageBox.addButton(
                    "بله، حذف شود",
                    QMessageBox::AcceptRole
                    );

            QPushButton* noButton =
                messageBox.addButton(
                    "انصراف",
                    QMessageBox::RejectRole
                    );

            messageBox.setDefaultButton(
                noButton
                );

            messageBox.exec();

            if (messageBox.clickedButton() != yesButton)
                return;

            m_maintenanceManager.removeAssetData(
                assetId
                );

            m_assetManager.removeAsset(
                assetId
                );

            refreshApplicationData();
        }
        );

    connect(
        m_assetSection,
        &AssetSection::openAssetRequested,
        this,
        [this](const QUuid& assetId)
        {
            qDebug() << "Tmrhome OPEN:" << assetId;

            for (int i = 0;
                 i < assetTree->topLevelItemCount();
                 ++i)
            {
                QTreeWidgetItem* item =
                    assetTree->topLevelItem(i);

                if (item->data(
                            0,
                            Qt::UserRole
                            ).toUuid() == assetId)
                {
                    qDebug() << "Tmrhome: FOUND ASSET";

                    assetTree->setCurrentItem(item);

                    // Trigger the normal Overview navigation
                    QPushButton* overviewButton =
                        findChild<QPushButton*>("overviewButton");

                    if (overviewButton)
                        overviewButton->click();

                    return;
                }
            }

            qDebug() << "Tmrhome: ASSET NOT FOUND";
        }
        );

    m_mainContentStack->addWidget(m_assetSection);

    // -------------------------------------------------
    // Timeline
    // -------------------------------------------------

    m_timelineWidget =
        new TimelineWidget(
            &m_maintenanceManager,
            &m_assetManager,
            m_mainContentStack
            );

    m_mainContentStack->addWidget(
        m_timelineWidget
        );

    // ------------------------------------------------
    // Report Section
    //-------------------------------------------------

    m_reportssection = new ReportsSection(
        m_maintenanceManager,
        parent
        );
    m_mainContentStack->addWidget(m_reportssection);

    //create Storage section
    qDebug() << "CONTENT: before StorageSection";

    m_storageSection = new StorageSection(
        m_storageManager,
        parent
        );

    connect(
        &m_storageManager,
        &StorageManager::partsUpdated,
        this,
        [this]()
        {
            if (!assetTree)
                return;

            auto* item =
                assetTree->currentItem();

            if (!item)
                return;

            const QUuid assetId =
                item->data(
                        0,
                        Qt::UserRole
                        ).toUuid();

            Asset* asset =
                m_assetManager.getAsset(
                    assetId
                    );

            if (!asset)
                return;

            refreshPartsPage(
                *asset
                );
        }
        );
    connect(
        &m_storageManager,
        &StorageManager::partsUpdated,
        this,
        [this]()
        {
            m_dataManager.save();
        }
        );

    m_mainContentStack->addWidget(m_storageSection);

    //Settings
    auto* settingsWidget = new SettingsWidget(
        m_dataManager,
        parent
        );
    m_mainContentStack->addWidget(settingsWidget);

    m_mainContentStack->setCurrentWidget(assetPanel);

    return m_mainContentStack;


}
QFrame* Tmrhome::createTreePanel(QWidget* parent)
{
    auto* panel = new QFrame(parent);
    panel->setObjectName("treePanel");
    panel->setMinimumWidth(200);
    panel->setMaximumWidth(400);
    panel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    panel->setStyleSheet(
        "QFrame {"
        "    background-color: #e3e3e3;"
        "}"
        );
    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(0);

    // Search bar
    auto* searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(6);

    auto* searchEdit = new QLineEdit(panel);
    searchEdit->setObjectName("assetSearchEdit");
    searchEdit->setPlaceholderText(QStringLiteral("جستجو..."));
    searchEdit->setLayoutDirection(Qt::RightToLeft);
    searchEdit->setClearButtonEnabled(true);

    auto* filterButton = new QToolButton(panel);
    filterButton->setObjectName("assetFilterButton");
    filterButton->setIcon(
        QIcon(":/qlementine/icons/16/action/filter.svg")
        );
    filterButton->setIconSize(QSize(18, 18));
    filterButton->setFixedSize(36, 36);
    filterButton->setToolTip(QStringLiteral("فیلتر تجهیزات"));

    searchLayout->addWidget(searchEdit, 1);
    searchLayout->addWidget(filterButton);

    layout->addLayout(searchLayout);

    // Asset and sub-asset tree
    assetTree = new AssetTree(panel);
    assetTree->setObjectName("assetTree");


    assetTree->setHeaderHidden(true);
    assetTree->setRootIsDecorated(true);
    assetTree->setIndentation(20);
    assetTree->setLayoutDirection(Qt::RightToLeft);
    assetTree->setFrameShape(QFrame::NoFrame);
    assetTree->setStyleSheet(
        "QTreeWidget {"
        "    background-color: #e3e3e3;"
        "    border: none;"
        "}"
        "QTreeWidget::item {"
        "    background-color: #e3e3e3;"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: #1890ff;"
        "}"
        );
    assetTree->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );

    layout->addWidget(assetTree, 1);

    return panel;
}

void Tmrhome::setupAssetTree()
{
    if (!assetTree)
        return;

    // Preserve the currently selected asset.
    QUuid selectedAssetId;

    if (QTreeWidgetItem* current = assetTree->currentItem())
    {
        selectedAssetId = QUuid(
            current->data(0, Qt::UserRole).toString()
            );
    }

    assetTree->clear();

    for (const Asset& asset : m_assetManager.assets())
    {
        auto* assetItem = new QTreeWidgetItem(assetTree);

        assetItem->setBackground(
            0, QBrush(QColor("#e3e3e3"))
            );
        assetItem->setText(0, asset.info().name);
        assetItem->setIcon(
            0,
            QIcon(":/qlementine/icons/16/instrument/machine.svg")
            );
        assetItem->setData(
            0,
            Qt::UserRole,
            asset.id().toString()
            );

        // Add sub-assets beneath their parent asset.
        for (const SubAsset& subAsset : asset.subAssets())
        {
            auto* subAssetItem = new QTreeWidgetItem(assetItem);

            subAssetItem->setText(0, subAsset.info().name);

            assetItem->setBackground(
                0, QBrush(QColor("#e3e3e3"))
                );
            subAssetItem->setIcon(
                0,
                QIcon(":/qlementine/icons/custome/gears.svg")
                );
            subAssetItem->setData(
                0,
                Qt::UserRole,
                asset.id().toString()
                );
        }

        if (asset.id() == selectedAssetId)
            assetTree->setCurrentItem(assetItem);
    }

    // Select the first asset if the previous selection no longer exists.
    if (!assetTree->currentItem() &&
        assetTree->topLevelItemCount() > 0)
    {
        assetTree->setCurrentItem(assetTree->topLevelItem(0));
    }

    assetTree->expandAll();
}

QWidget* Tmrhome::createAssetContentPanel(QWidget* parent)
{
    auto* panel = new QWidget(parent);

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Create the pages before the tabs connect to them.
    m_assetContentStack = createAssetPages(panel);

    layout->addWidget(createAssetHeader(panel));
    layout->addWidget(createAssetTabs(panel));
    layout->addWidget(m_assetContentStack, 1);

    return panel;
}

void Tmrhome::updateAssetContent(const Asset& asset)
{
    const AssetInfo& info = asset.info();

    // Update the asset header.
    m_assetNameLabel->setText(info.name);
    m_assetSerialLabel->setText(info.serialNumber);
    refreshPartsPage(asset);

    m_assetImageLabel->setPixmap(QPixmap());
    m_assetImageLabel->setText(QStringLiteral("تصویر"));

    if (!info.imagePath.isEmpty()) {
        QPixmap pixmap(info.imagePath);

        if (!pixmap.isNull()) {
            m_assetImageLabel->setText(QString());

            m_assetImageLabel->setPixmap(
                pixmap.scaled(
                    m_assetImageLabel->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    )
                );
        }
    }

    updateEventsForAsset(asset);
    updateSchedulesForAsset(asset);
    // IMPORTANT: Set the selected asset in the overview widget.
    m_overviewWidget->setAsset(asset);
    m_subAssetsWidget->setAsset(asset);
}
void Tmrhome::createDemoAssets()
{
    // Pump
    AssetInfo pumpInfo;
    pumpInfo.name = QStringLiteral("پمپ انتقال آب");
    pumpInfo.serialNumber = QStringLiteral("P-001");
    pumpInfo.manufacturer = QStringLiteral("Grundfos");
    pumpInfo.model = QStringLiteral("CR 32");

    Asset* pump = m_assetManager.addAsset(pumpInfo);

    if (pump) {
        SubAssetInfo motorInfo;
        motorInfo.name = QStringLiteral("الکتروموتور");

        m_assetManager.addSubAsset(pump->id(), motorInfo);

        SubAssetInfo couplingInfo;
        couplingInfo.name = QStringLiteral("کوپلینگ");

        m_assetManager.addSubAsset(pump->id(), couplingInfo);
    }

    // Sand filter
    AssetInfo filterInfo;
    filterInfo.name = QStringLiteral("فیلتر شنی");
    filterInfo.serialNumber = QStringLiteral("SF-001");
    filterInfo.manufacturer = QStringLiteral("Timar");
    filterInfo.model = QStringLiteral("SF-500");

    Asset* filter = m_assetManager.addAsset(filterInfo);

    if (filter) {
        SubAssetInfo valveInfo;
        valveInfo.name = QStringLiteral("شیر تخلیه");

        m_assetManager.addSubAsset(filter->id(), valveInfo);

        SubAssetInfo nozzleInfo;
        nozzleInfo.name = QStringLiteral("مجموعه نازل‌ها");

        m_assetManager.addSubAsset(filter->id(), nozzleInfo);
    }
}

QWidget* Tmrhome::createAssetHeader(QWidget* parent)
{
    auto* header = new QFrame(parent);
    header->setObjectName("assetHeader");
    header->setFixedHeight(110);
    header->setStyleSheet(
        "#assetHeader { background-color: white; border: none; }"
        );

    auto* layout = new QHBoxLayout(header);
    layout->setContentsMargins(20, 15, 25, 15);
    layout->setSpacing(15);

    auto* infoWidget = new QWidget(header);
    auto* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(4);

    m_assetNameLabel = new QLabel(
        QStringLiteral("یک تجهیز را انتخاب کنید"), infoWidget);
    m_assetNameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_assetNameLabel->setStyleSheet(
        "font-size: 18px; font-weight: 600; color: #222222;"
        );

    m_assetSerialLabel = new QLabel(infoWidget);
    m_assetSerialLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_assetSerialLabel->setStyleSheet(
        "font-size: 12px; color: #888888;"
        );

    infoLayout->addWidget(m_assetNameLabel);
    infoLayout->addWidget(m_assetSerialLabel);

    m_assetImageLabel = new QLabel(header);
    m_assetImageLabel->setFixedSize(75, 75);
    m_assetImageLabel->setAlignment(Qt::AlignCenter);
    m_assetImageLabel->setText(QStringLiteral("تصویر"));
    m_assetImageLabel->setStyleSheet(
        "background-color: #e3e3e3;"
        "border-radius: 8px;"
        "color: #777777;"
        );

    layout->addWidget(infoWidget, 1);
    layout->addWidget(m_assetImageLabel);

    return header;
}

QWidget* Tmrhome::createAssetTabs(QWidget* parent)
{
    auto* bar = new QFrame(parent);
    bar->setObjectName("assetTabBar");
    bar->setFixedHeight(40);
    bar->setStyleSheet(
        "#assetTabBar {"
        "background-color: white;"
        "border-bottom: 1px solid #dddddd;"
        "}"
        );

    auto* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(20, 0, 20, 0);
    layout->setSpacing(5);
    layout->setDirection(QBoxLayout::RightToLeft);

    auto createTab = [bar](const QString& text) {
        auto* button = new QPushButton(text, bar);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setFlat(true);
        button->setMinimumHeight(40);
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        button->setFocusPolicy(Qt::NoFocus);
        button->setStyleSheet(
            "QPushButton {"
            "border: none; background: transparent; color: #666666;"
            "padding: 0 14px; font-size: 13px;"
            "}"
            "QPushButton:hover { color: #222222; }"
            "QPushButton:checked {"
            "color: #1890ff; font-weight: 600;"
            "border-bottom: 2px solid #1890ff;"
            "}"
            );
        return button;
    };

    m_overviewTab  = createTab(QStringLiteral("شرح کلی"));
    m_historyTab   = createTab(QStringLiteral("تاریخچه"));
    m_subAssetsTab = createTab(QStringLiteral("تجهیزات جزء"));
    m_eventsTab    = createTab(QStringLiteral("رویدادها"));
    m_schedulesTab = createTab(QStringLiteral("برنامه‌ها"));
    m_partsTab     = createTab(QStringLiteral("قطعات"));

    auto* group = new QButtonGroup(bar);
    group->setExclusive(true);

    const QList<QPushButton*> tabs = {
        m_overviewTab,
        m_historyTab,
        m_subAssetsTab,
        m_eventsTab,
        m_schedulesTab,
        m_partsTab
    };

    for (auto* tab : tabs) {
        group->addButton(tab);
        layout->addWidget(tab);
    }

    layout->addStretch();
    m_overviewTab->setChecked(true);

    connect(m_overviewTab, &QPushButton::clicked,
            this, [this]() { m_assetContentStack->setCurrentIndex(0); });
    connect(m_historyTab, &QPushButton::clicked,
            this, [this]() { m_assetContentStack->setCurrentIndex(1); });
    connect(m_subAssetsTab, &QPushButton::clicked,
            this, [this]() { m_assetContentStack->setCurrentIndex(2); });
    connect(m_eventsTab, &QPushButton::clicked,
            this, [this]() { m_assetContentStack->setCurrentIndex(3); });
    connect(m_schedulesTab, &QPushButton::clicked,
            this, [this]() { m_assetContentStack->setCurrentIndex(4); });
    connect(m_partsTab, &QPushButton::clicked,
            this, [this]() { m_assetContentStack->setCurrentIndex(5); });

    return bar;
}

QStackedWidget* Tmrhome::createAssetPages(QWidget* parent)
{
    auto* stack = new QStackedWidget(parent);
    stack->setObjectName("assetContentStack");
    stack->setStyleSheet(
        "QStackedWidget { background-color: white; }"
        );
    // Overall
    m_overviewWidget =
        new AssetOverviewWidget(stack);
    connect(
        m_overviewWidget,
        &AssetOverviewWidget::deleteRequested,
        this,
        [this](const QUuid& assetId)
        {
            m_maintenanceManager.removeAssetData(assetId);

            m_assetManager.removeAsset(assetId);

            refreshApplicationData();
        }
        );

    stack->addWidget(
        m_overviewWidget
        );

    connect(
        m_overviewWidget,
        &AssetOverviewWidget::editRequested,
        this,
        [this]()
        {
            Asset* asset =
                m_assetManager.getAsset(
                    m_overviewWidget->assetId()
                    );

            if (!asset)
                return;

            AssetEditDialog dialog(
                asset->info(),
                this
                );

            if (dialog.exec() != QDialog::Accepted)
                return;

            if (!m_assetManager.updateAsset(
                    asset->id(),
                    dialog.assetInfo()))
            {
                return;
            }

            refreshApplicationData();
        }
        );
    auto createPlaceholder = [stack](const QString& title) {
        auto* page = new QWidget(stack);
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(25, 25, 25, 25);

        auto* label = new QLabel(title, page);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setStyleSheet(
            "font-size: 16px; font-weight: 600; color: #333333;"
            );

        layout->addWidget(label);
        layout->addStretch();

        return page;
    };

    //stack->addWidget(createPlaceholder(QStringLiteral("شرح کلی")));
    stack->addWidget(createPlaceholder(QStringLiteral("تاریخچه")));

    //add subAssets widget
    m_subAssetsWidget =
        new SubAssetsWidget(stack);

    connect(
        m_subAssetsWidget,
        &SubAssetsWidget::editSubAssetRequested,
        this,
        [this](
            const QUuid& assetId,
            const QUuid& subAssetId,
            const SubAssetInfo& info
            )
        {
            if (!m_assetManager.updateSubAsset(
                    assetId,
                    subAssetId,
                    info))
            {
                return;
            }

            refreshApplicationData();
        }
        );

    connect(
        m_subAssetsWidget,
        &SubAssetsWidget::deleteSubAssetRequested,
        this,
        [this](
            const QUuid& assetId,
            const QUuid& subAssetId
            )
        {
            const QMessageBox::StandardButton reply =
                QMessageBox::question(
                    this,
                    "حذف زیرتجهیز",
                    "آیا از حذف این زیرتجهیز مطمئن هستید؟\n"
                    "تمام برنامه‌ها و رویدادهای نگهداری مربوط به آن نیز حذف خواهند شد.",
                    QMessageBox::Yes |
                        QMessageBox::No,
                    QMessageBox::No
                    );

            if (reply != QMessageBox::Yes)
                return;

            m_maintenanceManager.removeSubAssetData(
                assetId,
                subAssetId
                );

            if (!m_assetManager.removeSubAsset(
                    assetId,
                    subAssetId))
            {
                return;
            }

            refreshApplicationData();
        }
        );

    stack->addWidget(
        m_subAssetsWidget
        );

    connect(
        m_subAssetsWidget,
        &SubAssetsWidget::addSubAssetRequested,
        this,
        [this](
            const QUuid& assetId,
            const SubAssetInfo& info)
        {
            qDebug() << "7 - Tmrhome received signal";
            qDebug() << "Asset ID:" << assetId;
            qDebug() << "SubAsset:" << info.name;

            const bool success =
                m_assetManager.addSubAsset(
                    assetId,
                    info
                    );

            qDebug() << "8 - addSubAsset result:" << success;

            if (!success)
                return;

            refreshApplicationData();
        }
        );

    stack->addWidget(createEventsPage(stack));
    stack->addWidget(createSchedulesContentPanel());
    stack->addWidget(createPartsPage(parent));


    return stack;
}

QWidget* Tmrhome::createEventsPage(QWidget* parent)
{
    auto* page = new QWidget(parent);

    auto* layout = new QVBoxLayout(page);

    layout->setContentsMargins(5, 15, 5, 15);
    layout->setSpacing(15);

    auto* addEventButton =
        new QPushButton("افزودن رویداد +", page);

    addEventButton->setFlat(true);
    addEventButton->setCursor(Qt::PointingHandCursor);

    addEventButton->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background: transparent;"
        "    color: #1890ff;"
        "    font-size: 13px;"
        "    font-weight: 600;"
        "}"
        ""
        "QPushButton:hover {"
        "    color: #096dd9;"
        "}"
        );

    connect(
        addEventButton,
        &QPushButton::clicked,
        this,
        &Tmrhome::openCreateEventDialog
        );

    layout->addWidget(addEventButton, 0, Qt::AlignRight);

    // Scroll area
    auto* scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollArea->setStyleSheet(
        "QScrollArea {"
        "    background-color: white;"
        "    border: none;"
        "}"
        );

    // Container inside scroll area
    auto* eventsContainer = new QWidget();

    eventsContainer->setObjectName("eventsContainer");

    eventsContainer->setStyleSheet(
        "QWidget#eventsContainer {"
        "    background-color: white;"
        "}"
        );

    // IMPORTANT:
    // Assign to the member variable, not a local variable.
    eventsLayout = new QVBoxLayout(eventsContainer);

    eventsLayout->setContentsMargins(0, 0, 0, 0);
    eventsLayout->setSpacing(10);
    eventsLayout->setAlignment(Qt::AlignTop | Qt::AlignRight);

    //eventsLayout->addStretch();

    scrollArea->setWidget(eventsContainer);

    layout->addWidget(scrollArea, 1);

    return page;
}

QWidget* Tmrhome::createSchedulesContentPanel()
{
    auto* page =
        new QWidget(this);

    auto* layout =
        new QVBoxLayout(page);

    layout->setContentsMargins(5, 15, 5, 15);
    layout->setSpacing(15);
    auto* addEventButton =
        new QPushButton("افزودن برنامه +", page);

    addEventButton->setFlat(true);
    addEventButton->setCursor(Qt::PointingHandCursor);

    addEventButton->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background: transparent;"
        "    color: #1890ff;"
        "    font-size: 13px;"
        "    font-weight: 600;"
        "}"
        ""
        "QPushButton:hover {"
        "    color: #096dd9;"
        "}"
        );

    connect(
        addEventButton,
        &QPushButton::clicked,
        this,
        &Tmrhome::createScheduleDialog
        );

    layout->addWidget(addEventButton, 0, Qt::AlignRight);


    //scrollArea->

    m_schedulesScrollArea =
        new QScrollArea(page);

    m_schedulesScrollArea->setWidgetResizable(true);
    m_schedulesScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );
    m_schedulesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_schedulesScrollArea->setFrameShape(QFrame::NoFrame);

    m_schedulesScrollArea->setStyleSheet(
        "QScrollArea {"
        "    background-color: white;"
        "    border: none;"
        "}"
        );


    auto* container =
        new QWidget();
    container->setObjectName("eventsContainer");

    container->setStyleSheet(
        "QWidget#eventsContainer {"
        "    background-color: white;"
        "}"
        );

    m_schedulesLayout =
        new QVBoxLayout(container);

    m_schedulesLayout->setContentsMargins(
        10, 10, 10, 10
        );

    m_schedulesLayout->setSpacing(10);
    m_schedulesLayout->setAlignment(
        Qt::AlignTop | Qt::AlignRight
        );
    m_schedulesScrollArea->setWidget(
        container
        );

    layout->addWidget(
        m_schedulesScrollArea, 1
        );

    return page;
}

void Tmrhome::refreshApplicationData()
{
    setupAssetTree();

    assetTree->expandAll();

   m_assetSection->refreshAssets();

    m_reportssection->refreshAssets();

    QTreeWidgetItem* currentItem =
        assetTree->currentItem();

    if (currentItem)
    {
        const QUuid assetId =
            currentItem->data(
                           0,
                           Qt::UserRole
                           ).toUuid();

        Asset* asset =
            m_assetManager.getAsset(assetId);

        if (asset)
        {
            updateAssetContent(*asset);
        }
    }

    m_dataManager.save();
}

void Tmrhome::openCreateEventDialog()
{
    auto* item = assetTree->currentItem();

    if (!item)
        return;

    const QUuid assetId(
        item->data(
                0,
                Qt::UserRole
                ).toString()
        );

    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (!asset)
        return;

    QList<MaintenanceSchedule> assetSchedules;

    for (const MaintenanceSchedule& schedule :
         m_maintenanceManager.schedules())
    {
        if (schedule.assetId == assetId)
            assetSchedules.append(schedule);
    }

    MaintenanceEventDialog dialog(
        asset->info().name,
        QString(),
        assetSchedules,
        this
        );

    if (dialog.exec() != QDialog::Accepted)
        return;

    MaintenanceEventData data =
        dialog.eventData();
    data.assetId = asset->id();
    data.facilitySection = asset->info().installationPlace;

    m_maintenanceManager.addEvent(data);

    refreshApplicationData();
}

void Tmrhome::updateEventsForAsset(const Asset& asset)
{
    while (QLayoutItem* item = eventsLayout->takeAt(0))
    {
        if (auto* widget = item->widget())
            widget->deleteLater();

        delete item;
    }

    for (const auto& event : m_maintenanceManager.events())
    {
        const auto& data = event.data();

        if (data.assetId != asset.id())
            continue;

        MaintenanceEventData displayData = data;

        if (!data.subAssetId.isNull())
        {
            for (const auto& subAsset : asset.subAssets())
            {
                if (subAsset.id() == data.subAssetId)
                {
                    displayData.installationPlace =
                        subAsset.info().installationPlace;

                    break;
                }
            }
        }
        else
        {
            displayData.installationPlace =
                asset.info().installationPlace;
        }

        auto* eventWidget =
            new MaintenanceEventWidget(displayData);

        eventsLayout->addWidget(
            eventWidget,
            0,
            Qt::AlignRight
            );

        connect(
            eventWidget,
            &MaintenanceEventWidget::statusChangeRequested,
            this,
            [this](
                const QUuid& eventId,
                MaintenanceStatus status
                )
            {
                if (status == MaintenanceStatus::Completed)
                {
                    m_maintenanceManager.completeEvent(
                        eventId,
                        QDate::currentDate()
                        );
                }
                else
                {
                    m_maintenanceManager.setEventStatus(
                        eventId,
                        status
                        );
                }
            }
            );

        connect(
            eventWidget,
            &MaintenanceEventWidget::deleteRequested,
            this,
            [this](const QUuid& eventId)
            {
                MaintenanceEvent* event =
                    m_maintenanceManager.getEvent(eventId);

                if (!event)
                    return;

                const MaintenanceEventData& data =
                    event->data();

                // -----------------------------------------------------
                // This event belongs to a maintenance schedule.
                // It cannot be deleted independently.
                // -----------------------------------------------------

                if (!data.scheduleId.isNull())
                {
                    QMessageBox::information(
                        this,
                        "حذف رویداد",
                        "این رویداد از یک برنامه نگهداری ایجاد شده است.\n\n"
                        "برای حذف این رویداد، ابتدا باید برنامه نگهداری "
                        "مربوط به آن را حذف کنید.\n\n"
                        "لطفاً به صفحه برنامه‌های نگهداری بروید.",
                        QMessageBox::Ok
                        );

                    return;
                }

                // -----------------------------------------------------
                // Normal event
                // -----------------------------------------------------

                const QMessageBox::StandardButton reply =
                    QMessageBox::question(
                        this,
                        "حذف رویداد",
                        "آیا از حذف این رویداد مطمئن هستید؟",
                        QMessageBox::Yes |
                            QMessageBox::No,
                        QMessageBox::No
                        );

                if (reply != QMessageBox::Yes)
                    return;

                m_maintenanceManager.removeEvent(
                    eventId
                    );
            }
            );
    }
}

void Tmrhome::refreshCurrentAssetEvents()
{
    auto* item = assetTree->currentItem();

    if (!item)
        return;

    const QUuid assetId(
        item->data(
                0,
                Qt::UserRole
                ).toString()
        );

    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (asset)
    {
        updateEventsForAsset(*asset);
    }
}

void Tmrhome::updateSchedulesForAsset(
    const Asset& asset
    )
{
    while (QLayoutItem* item =
           m_schedulesLayout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            widget->deleteLater();

        delete item;
    }

    const QUuid assetId = asset.id();

    for (const MaintenanceSchedule& schedule :
         m_maintenanceManager.schedules())
    {
        if (schedule.assetId != assetId)
            continue;

        auto* widget =
            new MaintenanceScheduleWidget(
                schedule
                );

        connect(
            widget,
            &MaintenanceScheduleWidget::statusChangeRequested,
            this,
            [this](
                const QUuid& scheduleId,
                MaintenanceScheduleStatus status
                )
            {
                m_maintenanceManager.setScheduleStatus(
                    scheduleId,
                    status
                    );
            }
            );
        connect(
            widget,
            &MaintenanceScheduleWidget::removeRequested,
            this,
            [this](const QUuid& scheduleId)
            {
                QMessageBox messageBox(this);

                messageBox.setWindowTitle(
                    "حذف برنامه"
                    );

                messageBox.setText(
                    "آیا از حذف این برنامه اطمینان دارید؟"
                    );

                messageBox.setInformativeText(
                    "رویدادهای در انتظار این برنامه نیز حذف خواهند شد."
                    );

                messageBox.setIcon(
                    QMessageBox::Warning
                    );

                QPushButton* yesButton =
                    messageBox.addButton(
                        "بله، حذف شود",
                        QMessageBox::AcceptRole
                        );

                QPushButton* noButton =
                    messageBox.addButton(
                        "انصراف",
                        QMessageBox::RejectRole
                        );

                messageBox.setDefaultButton(
                    noButton
                    );

                messageBox.exec();

                if (messageBox.clickedButton() != yesButton)
                    return;

                m_maintenanceManager.removeSchedule(
                    scheduleId
                    );
            }
            );
        m_schedulesLayout->addWidget(
            widget
            );
    }

    m_schedulesLayout->addStretch();
}

void Tmrhome::refreshCurrentAssetSchedules()
{
    auto* item = assetTree->currentItem();

    if (!item)
        return;

    const QUuid assetId(
        item->data(
                0,
                Qt::UserRole
                ).toString()
        );

    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (asset)
    {
        updateSchedulesForAsset(*asset);
    }
}

void Tmrhome::createScheduleDialog()
{
    auto* item = assetTree->currentItem();

    if (!item)
        return;

    const QUuid assetId(
        item->data(
                0,
                Qt::UserRole
                ).toString()
        );

    Asset* asset =
        m_assetManager.getAsset(assetId);

    if (!asset)
        return;

    MaintenanceScheduleDialog dialog(
        asset->id(),
        asset->info().name,
        asset->subAssets(),
        m_storageManager,
        this
        );

    if (dialog.exec() == QDialog::Accepted)
    {
        m_maintenanceManager.addSchedule(
            dialog.scheduleData()
            );
    }
}

QWidget* Tmrhome::createPartsPage(QWidget* parent)
{
    auto* page =
        new QWidget(parent);

    auto* layout =
        new QVBoxLayout(page);

    layout->setContentsMargins(
        25, 20, 25, 20
        );
    layout->setSpacing(10);

    auto* headerLayout =
        new QHBoxLayout();

    auto* title =
        new QLabel(
            "قطعات تجهیز",
            page
            );

    title->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 600;"
        "color: #333333;"
        );

    m_addPartButton =
        new QPushButton(
            "افزودن قطعه",
            page
            );

    m_addPartButton->setMinimumHeight(36);

    connect(
        m_addPartButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            auto* item =
                assetTree->currentItem();

            if (!item)
                return;

            const QUuid assetId =
                item->data(
                        0,
                        Qt::UserRole
                        ).toUuid();

            Asset* asset =
                m_assetManager.getAsset(
                    assetId
                    );

            if (!asset)
                return;

            QDialog dialog(this);

            dialog.setWindowTitle(
                "افزودن قطعه"
                );

            dialog.setMinimumWidth(
                400
                );

            auto* layout =
                new QVBoxLayout(&dialog);

            auto* partLabel =
                new QLabel(
                    "قطعه",
                    &dialog
                    );

            auto* partCombo =
                new QComboBox(
                    &dialog
                    );

            for (const StoragePart& part :
                 m_storageManager.parts())
            {
                partCombo->addItem(
                    part.name +
                        " (" +
                        part.partNumber +
                        ") - موجودی: " +
                        QString::number(
                            part.quantity
                            ),
                    part.id
                    );
            }

            layout->addWidget(
                partLabel
                );

            layout->addWidget(
                partCombo
                );

            auto* buttonLayout =
                new QHBoxLayout();

            auto* cancelButton =
                new QPushButton(
                    "انصراف",
                    &dialog
                    );

            auto* addButton =
                new QPushButton(
                    "افزودن",
                    &dialog
                    );

            buttonLayout->addWidget(
                cancelButton
                );

            buttonLayout->addWidget(
                addButton
                );

            layout->addLayout(
                buttonLayout
                );

            connect(
                cancelButton,
                &QPushButton::clicked,
                &dialog,
                &QDialog::reject
                );

            connect(
                addButton,
                &QPushButton::clicked,
                &dialog,
                &QDialog::accept
                );

            if (partCombo->count() == 0)
            {
                addButton->setEnabled(false);

                auto* emptyLabel =
                    new QLabel(
                        "هیچ قطعه‌ای در انبار موجود نیست.",
                        &dialog
                        );

                emptyLabel->setStyleSheet(
                    "color: #d32f2f;"
                    );

                layout->insertWidget(
                    1,
                    emptyLabel
                    );
            }

            if (dialog.exec() != QDialog::Accepted)
                return;

            const QUuid storagePartId =
                partCombo->currentData().toUuid();

            if (m_storageManager.installPart(
                    assetId,
                    storagePartId
                    ))
            {
                refreshPartsPage(
                    *asset
                    );

                m_dataManager.save();
            }
        }
        );

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(
        m_addPartButton
        );

    layout->addLayout(
        headerLayout
        );

    auto* scrollArea =
        new QScrollArea(page);

    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(
        QFrame::NoFrame
        );

    QPalette palette =
        scrollArea->palette();

    palette.setColor(
        QPalette::Base,
        Qt::white
        );

    scrollArea->setPalette(
        palette
        );

    scrollArea->viewport()->setPalette(
        palette
        );

    auto* container =
        new QWidget();

    container->setAutoFillBackground(
        true
        );

    QPalette containerPalette =
        container->palette();

    containerPalette.setColor(
        QPalette::Window,
        Qt::white
        );

    container->setPalette(
        containerPalette
        );

    m_partsLayout =
        new QVBoxLayout(container);

    m_partsLayout->setContentsMargins(
        0, 5, 0, 5
        );

    m_partsLayout->setSpacing(8);

    m_partsLayout->addStretch();

    scrollArea->setWidget(
        container
        );

    layout->addWidget(
        scrollArea,
        1
        );

    return page;
}

void Tmrhome::refreshPartsPage(
    const Asset& asset
    )
{
    if (!m_partsLayout)
        return;

    while (m_partsLayout->count() > 1)
    {
        QLayoutItem* item =
            m_partsLayout->takeAt(0);

        if (item->widget())
            item->widget()->deleteLater();

        delete item;
    }

    for (const AssetPart& assetPart :
         asset.info().parts)
    {
        m_partsLayout->insertWidget(
            m_partsLayout->count() - 1,
            createPartRow(
                assetPart,
                asset
                )
            );
    }

    if (asset.info().parts.isEmpty())
    {
        auto* label =
            new QLabel(
                "هیچ قطعه‌ای برای این تجهیز ثبت نشده است.",
                m_partsLayout->parentWidget()
                );

        label->setAlignment(
            Qt::AlignCenter
            );

        label->setStyleSheet(
            "color: #888888;"
            "padding: 30px;"
            );

        m_partsLayout->insertWidget(
            0,
            label
            );
    }
}

QWidget* Tmrhome::createPartRow(
    const AssetPart& assetPart,
    const Asset& asset
    )
{
    auto* frame =
        new QFrame();

    frame->setObjectName(
        "assetPartRow"
        );

    frame->setFrameShape(
        QFrame::StyledPanel
        );

    frame->setFrameShadow(
        QFrame::Plain
        );

    frame->setMinimumHeight(
        85
        );

    frame->setStyleSheet(
        "QFrame#assetPartRow {"
        "background-color: #ffffff;"
        "border: 1px solid #e2e8f0;"
        "border-radius: 8px;"
        "}"
        );

    auto* layout =
        new QHBoxLayout(frame);

    layout->setContentsMargins(
        15, 10, 15, 10
        );
    layout->setDirection(QBoxLayout::RightToLeft);
    layout->setSpacing(15);

    auto* infoLayout =
        new QVBoxLayout();

    const StoragePart* storagePart =
        m_storageManager.getPart(
            assetPart.storagePartId
            );

    QString name =
        storagePart
            ? storagePart->name
            : "قطعه حذف شده";

    QString partNumber =
        storagePart
            ? storagePart->partNumber
            : "-";

    auto* nameLabel =
        new QLabel(name);

    nameLabel->setStyleSheet(
        "font-weight: 600;"
        "font-size: 14px;"
        "color: #333333;"
        );

    auto* numberLabel =
        new QLabel(
            "شماره قطعه: " +
            partNumber
            );

    numberLabel->setStyleSheet(
        "color: #666666;"
        );

    infoLayout->addWidget(
        nameLabel
        );

    infoLayout->addWidget(
        numberLabel
        );

    layout->addLayout(
        infoLayout,
        1
        );

    auto* dateLayout =
        new QVBoxLayout();

    dateLayout->setContentsMargins(
        10, 0, 10, 0
        );

    dateLayout->setSpacing(4);

    auto* dateIconLabel =
        new QLabel();

    dateIconLabel->setFixedSize(
        24,
        24
        );

    dateIconLabel->setPixmap(
        QPixmap(":/qlementine/icons/custome/calendar.svg")
            .scaled(
                20,
                20,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
        );

    auto* dateLabel =
        new QLabel(
            toPersianDate(
                assetPart.installationDate
                )
            );

    dateLabel->setStyleSheet(
        "color: #888888;"
        );

    dateLayout->addWidget(
        dateIconLabel,
        0,
        Qt::AlignCenter
        );

    dateLayout->addWidget(
        dateLabel,
        0,
        Qt::AlignCenter
        );

    layout->addLayout(
        dateLayout
        );

    auto* quantityLayout =
        new QVBoxLayout();

    quantityLayout->setContentsMargins(
        10, 0, 10, 0
        );

    quantityLayout->setSpacing(4);

    auto* quantityIconLabel =
        new QLabel();

    quantityIconLabel->setFixedSize(
        24,
        24
        );

    quantityIconLabel->setPixmap(
        QPixmap(":/icons/storage.svg")
            .scaled(
                20,
                20,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
        );

    auto* quantityLabel =
        new QLabel(
            "موجودی: " +
            QString::number(
                storagePart
                    ? storagePart->quantity
                    : 0
                )
            );

    quantityLabel->setStyleSheet(
        "color: #666666;"
        );

    quantityLayout->addWidget(
        quantityIconLabel,
        0,
        Qt::AlignCenter
        );

    quantityLayout->addWidget(
        quantityLabel,
        0,
        Qt::AlignCenter
        );

    layout->addLayout(
        quantityLayout
        );

    if (storagePart &&
        storagePart->quantity == 0)
    {
        auto* warningLabel =
            new QLabel(
                "موجودی این قطعه در انبار تمام شده است."
                );

        warningLabel->setStyleSheet(
            "color: #d32f2f;"
            "font-size: 12px;"
            );

        layout->addWidget(
            warningLabel
            );
    }

    auto* removeButton =
        new QPushButton(
            "حذف"
            );

    auto* replaceButton =
        new QPushButton(
            "تعویض"
            );

    removeButton->setMinimumHeight(
        32
        );

    replaceButton->setMinimumHeight(
        32
        );

    layout->addWidget(
        replaceButton
        );

    connect(
        replaceButton,
        &QPushButton::clicked,
        this,
        [this, assetPart, &asset]()
        {
            if (m_storageManager.replacePart(
                    asset.id(),
                    assetPart.id
                    ))
            {
                refreshPartsPage(
                    asset
                    );

                m_dataManager.save();
            }
            else
            {
                QMessageBox::warning(
                    this,
                    "تعویض قطعه",
                    "قطعه مشابهی با موجودی کافی در انبار وجود ندارد."
                    );
            }
        }
        );

    layout->addWidget(
        removeButton
        );

    connect(
        removeButton,
        &QPushButton::clicked,
        this,
        [this, assetPart, &asset]()
        {
            if (m_storageManager.removePartFromAsset(
                    asset.id(),
                    assetPart.id
                    ))
            {
                refreshPartsPage(
                    asset
                    );

                m_dataManager.save();
            }
        }
        );

    return frame;
}

