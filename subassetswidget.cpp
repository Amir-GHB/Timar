#include "subassetswidget.h"
#include "asset.h"
#include "persiandateedit.h"
#include "persiandigits.h"
#include "subasset.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QPixmap>
#include <QSizePolicy>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qdebug.h>
#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QMouseEvent>
#include <QPixmap>
#include <qtextedit.h>

class SubAssetEditDialog : public QDialog
{
public:

    explicit SubAssetEditDialog(
        const SubAssetInfo& info,
        QWidget* parent = nullptr
        )
        : QDialog(parent)
    {
        setWindowTitle("ویرایش زیرتجهیز");
        setMinimumWidth(500);

        setupUI();

        // Fill existing information
        m_nameEdit->setText(info.name);
        m_typeEdit->setText(info.type);
        m_serialEdit->setText(info.serialNumber);
        m_manufacturerEdit->setText(info.manufacturer);
        m_modelEdit->setText(info.model);

        if (info.installationDate.isValid())
        {
            m_installationDateEdit->setDate(
                info.installationDate
                );
        }

        m_installationPlaceEdit->setText(
            info.installationPlace
            );

        m_imagePathEdit->setText(
            info.imagePath
            );

        m_descriptionEdit->setPlainText(
            info.description
            );

        const int statusIndex =
            testCombo->findText(info.status);

        if (statusIndex >= 0)
        {
            testCombo->setCurrentIndex(statusIndex);
        }
    }

    explicit SubAssetEditDialog(
        QWidget* parent = nullptr
        )
        : QDialog(parent)
    {
        setWindowTitle("افزودن زیرتجهیز");
        setMinimumWidth(500);

        setupUI();
    }

    SubAssetInfo subAssetInfo() const
    {
        SubAssetInfo info;

        info.name =
            m_nameEdit->text();

        info.type =
            m_typeEdit->text();

        info.status =
            testCombo->currentText();

        info.serialNumber =
            m_serialEdit->text();

        info.manufacturer =
            m_manufacturerEdit->text();

        info.model =
            m_modelEdit->text();

        info.installationDate =
            m_installationDateEdit->date();

        info.installationPlace =
            m_installationPlaceEdit->text();

        info.imagePath =
            m_imagePathEdit->text();

        info.description =
            m_descriptionEdit->toPlainText();

        return info;
    }

private:

    void setupUI()
    {
        qDebug() << "UI 1";

        auto* mainLayout =
            new QVBoxLayout(this);

        mainLayout->setContentsMargins(
            20,
            20,
            20,
            20
            );

        mainLayout->setSpacing(12);
        qDebug() << "UI 2";


        auto* formLayout =
            new QFormLayout();

        formLayout->setSpacing(10);

        formLayout->setLabelAlignment(
            Qt::AlignRight
            );

        qDebug() << "UI 3";

        // Name

        m_nameEdit =
            new QLineEdit(this);

        m_nameEdit->setPlaceholderText(
            "نام زیرتجهیز"
            );

        auto* nameWidget = new QWidget(this);
        auto* nameLayout = new QHBoxLayout(nameWidget);

        nameLayout->setContentsMargins(0, 0, 0, 0);
        nameLayout->setSpacing(4);

        m_nameRequiredLabel =
            createRequiredLabel(nameWidget);

        nameLayout->addWidget(m_nameRequiredLabel);
        nameLayout->addWidget(m_nameEdit);

        formLayout->addRow(
            "نام:",
            nameWidget
            );

        qDebug() << "UI 4";

        // Type

        m_typeEdit =
            new QLineEdit(this);

        m_typeEdit->setPlaceholderText(
            "نوع زیرتجهیز"
            );

        auto* typeWidget = new QWidget(this);
        auto* typeLayout = new QHBoxLayout(typeWidget);

        typeLayout->setContentsMargins(0, 0, 0, 0);
        typeLayout->setSpacing(4);

        m_typeRequiredLabel =
            createRequiredLabel(typeWidget);

        typeLayout->addWidget(m_typeRequiredLabel);
        typeLayout->addWidget(m_typeEdit);

        formLayout->addRow(
            "نوع:",
            typeWidget
            );

        qDebug() << "UI 5";

        // Status
        qDebug() << "UI 5.1";
        testCombo =
            new QComboBox(this);
        // m_statusCombo->setParent(this);
        /*        qDebug() << "combo ptr:" << m_statusCombo;
        qDebug() << "combo parent:" << m_statusCombo->parent();
        qDebug() << "combo count before:" << m_statusCombo->count();

        //m_statusCombo->addItem(QStringLiteral("فعال"));


        qDebug() << "combo count after:" << m_statusCombo->count();
*/
        testCombo->addItem("فعال");
        testCombo->addItem("غیرفعال");
        testCombo->addItem("در تعمیر");
        testCombo->addItem("خارج از سرویس");
        qDebug() << "UI 5.3";
        formLayout->addRow(
            "وضعیت:",
            testCombo
            );

        qDebug() << "UI 6";

        // Serial

        m_serialEdit =
            new QLineEdit(this);

        formLayout->addRow(
            "شماره سریال:",
            m_serialEdit
            );

        qDebug() << "UI 7";

        // Manufacturer

        m_manufacturerEdit =
            new QLineEdit(this);

        formLayout->addRow(
            "سازنده:",
            m_manufacturerEdit
            );

        qDebug() << "UI 8";

        // Model

        m_modelEdit =
            new QLineEdit(this);

        formLayout->addRow(
            "مدل:",
            m_modelEdit
            );


        // Installation date

        m_installationDateEdit =
            new PersianDateEdit(this);

        m_installationDateEdit->setCalendarPopup(
            true
            );

        m_installationDateEdit->setDate(
            (QDate::currentDate())
            );

        formLayout->addRow(
            "تاریخ نصب:",
            m_installationDateEdit
            );


        // Installation place

        m_installationPlaceEdit =
            new QLineEdit(this);

        formLayout->addRow(
            "محل نصب:",
            m_installationPlaceEdit
            );


        // Image

        auto* imageWidget =
            new QWidget(this);

        auto* imageLayout =
            new QHBoxLayout(imageWidget);

        imageLayout->setContentsMargins(
            0,
            0,
            0,
            0
            );

        m_imagePathEdit =
            new QLineEdit(imageWidget);

        auto* browseButton =
            new QPushButton(
                "انتخاب",
                imageWidget
                );

        imageLayout->addWidget(
            m_imagePathEdit
            );

        imageLayout->addWidget(
            browseButton
            );

        formLayout->addRow(
            "تصویر:",
            imageWidget
            );

        qDebug() << "UI 9";

        connect(
            browseButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                const QString path =
                    QFileDialog::getOpenFileName(
                        this,
                        "انتخاب تصویر",
                        QString(),
                        "Images (*.png *.jpg *.jpeg *.bmp *.gif *.webp *.tif *.tiff);;All Files (*)"
                        );

                if (!path.isEmpty())
                {
                    m_imagePathEdit->setText(
                        path
                        );
                }
            }
            );

 qDebug() << "UI 10";
        // Description

        m_descriptionEdit =
            new QTextEdit(this);
qDebug() << "UI 10.1";
        m_descriptionEdit->setMinimumHeight(
            80
            );
qDebug() << "UI 10.2";
        formLayout->addRow(
            "توضیحات:",
            m_descriptionEdit
            );

qDebug() << "UI 10.3";
        mainLayout->addLayout(
            formLayout
            );

qDebug() << "UI 11";
        // Buttons

        auto* buttons =
            new QDialogButtonBox(
                QDialogButtonBox::Ok |
                    QDialogButtonBox::Cancel,
                this
                );

        buttons->button(
                   QDialogButtonBox::Ok
                   )->setText(
                "ذخیره"
                );

        buttons->button(
                   QDialogButtonBox::Cancel
                   )->setText(
                "انصراف"
                );

qDebug() << "UI 12";
        connect(
            buttons,
            &QDialogButtonBox::accepted,
            this,
            [this]()
            {
                const bool nameEmpty =
                    m_nameEdit->text().trimmed().isEmpty();

                const bool typeEmpty =
                    m_typeEdit->text().trimmed().isEmpty();

                m_nameRequiredLabel->setVisible(nameEmpty);
                m_typeRequiredLabel->setVisible(typeEmpty);

                if (nameEmpty)
                {
                    m_nameEdit->setFocus();
                    return;
                }

                if (typeEmpty)
                {
                    m_typeEdit->setFocus();
                    return;
                }

                accept();
            }
            );

        connect(
            buttons,
            &QDialogButtonBox::rejected,
            this,
            &QDialog::reject
            );


        mainLayout->addWidget(
            buttons
            );
        qDebug() << "UI 13";
    }

    QLabel* createRequiredLabel(QWidget* parent)
    {
        auto* label = new QLabel("!", parent);

        label->setStyleSheet(
            "QLabel {"
            "    color: #d32f2f;"
            "    font-weight: bold;"
            "    font-size: 16px;"
            "}"
            );

        label->setVisible(false);

        return label;
    }

private:
    QLineEdit* m_nameEdit = nullptr;
    QLineEdit* m_typeEdit = nullptr;
    QComboBox* testCombo = nullptr;

    QLineEdit* m_serialEdit = nullptr;
    QLineEdit* m_manufacturerEdit = nullptr;
    QLineEdit* m_modelEdit = nullptr;

    QDateEdit* m_installationDateEdit = nullptr;
    QLineEdit* m_installationPlaceEdit = nullptr;

    QLineEdit* m_imagePathEdit = nullptr;

    QTextEdit* m_descriptionEdit = nullptr;

    QLabel* m_nameRequiredLabel = nullptr;
    QLabel* m_typeRequiredLabel = nullptr;
};

class SubAssetItemWidget : public QFrame
{
public:
    explicit SubAssetItemWidget(
        const SubAsset& subAsset,
        QWidget* parent = nullptr
        )
        : QFrame(parent),
        m_subAsset(subAsset)
    {
        setupUI();
    }

    QUuid subAssetId() const
    {
        return m_subAsset.id();
    }

    void setSelected(bool selected)
    {
        if (selected)
        {
            setStyleSheet(
                "#subAssetItem {"
                "    background-color: #eaf3ff;"
                "    border: 1px solid #1890ff;"
                "    border-radius: 8px;"
                "}"
                );
        }
        else
        {
            setStyleSheet(
                "#subAssetItem {"
                "    background-color: white;"
                "    border: 1px solid #e5e5e5;"
                "    border-radius: 8px;"
                "}"
                "#subAssetItem:hover {"
                "    background-color: #f7f9fb;"
                "}"
                );
        }
    }

    std::function<void(const QUuid&)> onClicked;
    std::function<void(const QUuid&)> onEditRequested;
    std::function<void(const QUuid&)> onDeleteRequested;

protected:
    void mousePressEvent(
        QMouseEvent* event
        ) override
    {
        if (event->button() == Qt::LeftButton)
        {
            if (onClicked)
                onClicked(m_subAsset.id());
        }

        QFrame::mousePressEvent(event);
    }

private:

    void setupUI()
    {
        setObjectName("subAssetItem");

        setMinimumHeight(96);

        setCursor(
            Qt::PointingHandCursor
            );


        auto* mainLayout =
            new QHBoxLayout(this);

        mainLayout->setContentsMargins(
            12,
            10,
            8,
            10
            );

        mainLayout->setSpacing(10);

        mainLayout->setDirection(
            QBoxLayout::LeftToRight
            );


        // =====================================================
        // Information
        // =====================================================

        auto* infoWidget =
            new QWidget(this);

        infoWidget->setLayoutDirection(
            Qt::RightToLeft
            );


        auto* infoLayout =
            new QVBoxLayout(infoWidget);

        infoLayout->setContentsMargins(
            0,
            0,
            0,
            0
            );

        infoLayout->setSpacing(3);


        // -----------------------------------------------------
        // Name
        // -----------------------------------------------------

        auto* nameLabel =
            new QLabel(
                m_subAsset.info().name,
                infoWidget
                );

        nameLabel->setAlignment(
            Qt::AlignLeft |
            Qt::AlignVCenter
            );

        nameLabel->setStyleSheet(
            "font-size: 14px;"
            "font-weight: 600;"
            "color: #222222;"
            );


        // -----------------------------------------------------
        // Type
        // -----------------------------------------------------

        auto* typeLabel =
            new QLabel(
                m_subAsset.info().type.isEmpty()
                    ? "-"
                    : m_subAsset.info().type,
                infoWidget
                );

        typeLabel->setAlignment(
            Qt::AlignLeft |
            Qt::AlignVCenter
            );

        typeLabel->setStyleSheet(
            "font-size: 12px;"
            "color: #777777;"
            );


        // -----------------------------------------------------
        // Bottom information row
        // -----------------------------------------------------

        auto* bottomWidget =
            new QWidget(infoWidget);

        bottomWidget->setLayoutDirection(
            Qt::RightToLeft
            );


        auto* bottomLayout =
            new QHBoxLayout(bottomWidget);

        bottomLayout->setContentsMargins(
            0,
            0,
            0,
            0
            );

        bottomLayout->setSpacing(12);


        // Status

        auto* statusLabel =
            new QLabel(
                infoWidget
                );

        statusLabel->setText(
            m_subAsset.info().status.isEmpty()
                ? "-"
                : m_subAsset.info().status
            );

        statusLabel->setAlignment(
            Qt::AlignRight |
            Qt::AlignVCenter
            );

        statusLabel->setStyleSheet(
            "font-size: 11px;"
            "color: #555555;"
            );


        // Installation date

        auto* dateLabel =
            new QLabel(
                infoWidget
                );

        if (m_subAsset.info()
                .installationDate
                .isValid())
        {
            dateLabel->setText(
                toPersianDate(m_subAsset.info()
                                  .installationDate)
                );
        }
        else
        {
            dateLabel->setText("-");
        }

        dateLabel->setAlignment(
            Qt::AlignRight |
            Qt::AlignVCenter
            );

        dateLabel->setStyleSheet(
            "font-size: 11px;"
            "color: #888888;"
            );


        bottomLayout->addWidget(
            statusLabel
            );

        bottomLayout->addWidget(
            dateLabel
            );

        bottomLayout->addStretch();


        // Add everything

        infoLayout->addWidget(
            nameLabel
            );

        infoLayout->addWidget(
            typeLabel
            );

        infoLayout->addWidget(
            bottomWidget
            );


        mainLayout->addWidget(
            infoWidget,
            1
            );


        // =========================================================
        // Action buttons
        // =========================================================

        auto* actionLayout =
            new QVBoxLayout();

        actionLayout->setContentsMargins(
            0,
            0,
            0,
            0
            );

        actionLayout->setSpacing(4);


        // =========================================================
        // Delete button
        // =========================================================

        auto* deleteButton =
            new QToolButton(this);

        deleteButton->setFixedSize(
            26,
            26
            );

        deleteButton->setCursor(
            Qt::PointingHandCursor
            );

        deleteButton->setIcon(
            QIcon(":/qlementine/icons/custome/trash-red.svg")
            );

        deleteButton->setIconSize(
            QSize(14, 14)
            );

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
                if (onDeleteRequested)
                {
                    onDeleteRequested(
                        m_subAsset.id()
                        );
                }
            }
            );


        // =========================================================
        // Edit button
        // =========================================================

        auto* editButton =
            new QToolButton(this);

        editButton->setFixedSize(
            26,
            26
            );

        editButton->setCursor(
            Qt::PointingHandCursor
            );

        editButton->setIcon(
            QIcon(":/qlementine/icons/custome/pen-green.svg")
            );

        editButton->setIconSize(
            QSize(14, 14)
            );

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
                if (onEditRequested)
                {
                    onEditRequested(
                        m_subAsset.id()
                        );
                }
            }
            );


        // =========================================================
        // Add buttons to layout
        // =========================================================

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

        mainLayout->addLayout(
            actionLayout
            );


        setSelected(false);
    }

private:
    SubAsset m_subAsset;
};

SubAssetsWidget::SubAssetsWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}


void SubAssetsWidget::setupUI()
{
    auto* mainLayout = new QHBoxLayout(this);

    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    /*
     * We explicitly use LeftToRight here so that:
     *
     * LEFT  = subasset list
     * RIGHT = detail panel
     *
     * Text inside the widgets can still be RTL.
     */
    mainLayout->setDirection(
        QBoxLayout::LeftToRight
        );


    // ---------------------------------------------------------
    // Subasset list
    // ---------------------------------------------------------

    auto* listPanel = createListPanel(this);
    listPanel->setMinimumWidth(280);

    mainLayout->addWidget(
        listPanel,
        3
        );


    // ---------------------------------------------------------
    // Detail panel
    // ---------------------------------------------------------

    auto* detailPanel = createDetailPanel(this);
    detailPanel->setMinimumWidth(0);
    mainLayout->addWidget(
        detailPanel,
        2
        );
}


QWidget* SubAssetsWidget::createListPanel(
    QWidget* parent
    )
{
    auto* panel = new QFrame(parent);

    panel->setObjectName(
        "subAssetsListPanel"
        );

    panel->setStyleSheet(
        "#subAssetsListPanel {"
        "    background-color: #f5f5f5;"
        "    border: 1px solid #e5e5e5;"
        "    border-radius: 10px;"
        "}"
        );


    auto* layout =
        new QVBoxLayout(panel);

    layout->setContentsMargins(
        10,
        10,
        10,
        10
        );

    layout->setSpacing(8);


    // =========================================================
    // Top bar
    // =========================================================

    auto* topBar =
        new QWidget(panel);

    auto* topBarLayout =
        new QHBoxLayout(topBar);

    topBarLayout->setContentsMargins(
        4,
        4,
        4,
        4
        );

    topBarLayout->setSpacing(8);

    /*
     * Physical layout:
     *
     * [ Title + Description ]     [ Add ]
     */

    topBarLayout->setDirection(
        QBoxLayout::RightToLeft
        );


    // ---------------------------------------------------------
    // Title + description
    // ---------------------------------------------------------

    auto* textWidget =
        new QWidget(topBar);

    textWidget->setLayoutDirection(
        Qt::RightToLeft
        );


    auto* textLayout =
        new QVBoxLayout(textWidget);

    textLayout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    textLayout->setSpacing(2);


    auto* titleLabel =
        new QLabel(
            "زیر تجهیزات",
            textWidget
            );

    titleLabel->setAlignment(
        Qt::AlignLeft |
        Qt::AlignVCenter
        );

    titleLabel->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 600;"
        "color: #222222;"
        );


    auto* descriptionLabel =
        new QLabel(
            "لیست زیر تجهیزات و اجزای تشکیل دهنده",
            textWidget
            );

    descriptionLabel->setAlignment(
        Qt::AlignLeft |
        Qt::AlignVCenter
        );

    descriptionLabel->setStyleSheet(
        "font-size: 11px;"
        "color: #888888;"
        );


    textLayout->addWidget(
        titleLabel
        );

    textLayout->addWidget(
        descriptionLabel
        );


    topBarLayout->addWidget(
        textWidget,
        1
        );


    // ---------------------------------------------------------
    // Add button
    // ---------------------------------------------------------

    auto* addButton =
        new QPushButton(
            "+ افزودن زیرتجهیز",
            topBar
            );

    addButton->setMinimumHeight(
        34
        );

    addButton->setCursor(
        Qt::PointingHandCursor
        );


    connect(
        addButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            qDebug() << "1 - Add button clicked";

            qDebug() << "Before dialog construction";
            SubAssetEditDialog dialog(window());
            qDebug() << "After dialog construction";

            qDebug() << "2 - Dialog constructed";

            const int result = dialog.exec();

            qDebug() << "3 - Dialog closed, result =" << result;

            if (result != QDialog::Accepted)
                return;

            qDebug() << "4 - Dialog accepted";

            const SubAssetInfo info =
                dialog.subAssetInfo();

            qDebug() << "5 - Info extracted:"
                     << info.name;

            emit addSubAssetRequested(
                m_assetId,
                info
                );

            qDebug() << "6 - Signal emitted";
        }
        );

    topBarLayout->addWidget(
        addButton
        );


    layout->addWidget(
        topBar
        );


    // =========================================================
    // List
    // =========================================================

    auto* scrollArea =
        new QScrollArea(panel);

    scrollArea->setWidgetResizable(
        true
        );

    scrollArea->setFrameShape(
        QFrame::NoFrame
        );

    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );


    auto* container =
        new QWidget();


    container->setObjectName(
        "subAssetsListContainer"
        );


    m_listLayout =
        new QVBoxLayout(container);

    m_listLayout->setContentsMargins(
        0,
        4,
        0,
        4
        );

    m_listLayout->setSpacing(8);

    m_listLayout->setAlignment(
        Qt::AlignTop
        );


    scrollArea->setWidget(
        container
        );


    layout->addWidget(
        scrollArea,
        1
        );


    return panel;
}


QWidget* SubAssetsWidget::createDetailPanel(
    QWidget* parent
    )
{
    auto* panel = new QFrame(parent);

    panel->setObjectName(
        "subAssetDetailPanel"
        );

    panel->setStyleSheet(
        "#subAssetDetailPanel {"
        "    background-color: white;"
        "    border: 1px solid #e5e5e5;"
        "    border-radius: 10px;"
        "}"
        );


    auto* layout = new QVBoxLayout(panel);

    layout->setContentsMargins(
        18,
        18,
        18,
        18
        );

    layout->setSpacing(16);


    // =========================================================
    // Header
    // =========================================================

    auto* header = new QWidget(panel);

    auto* headerLayout =
        new QHBoxLayout(header);

    headerLayout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    headerLayout->setSpacing(12);

    /*
     * Keep this physically LeftToRight:
     *
     * IMAGE | NAME / STATUS
     */
    headerLayout->setDirection(
        QBoxLayout::LeftToRight
        );


    // ---------------------------------------------------------
    // Image
    // ---------------------------------------------------------

    m_imageLabel = new QLabel(header);

    m_imageLabel->setFixedSize(
        140,
        110
        );

    m_imageLabel->setAlignment(
        Qt::AlignCenter
        );

    m_imageLabel->setStyleSheet(
        "background-color: #f2f2f2;"
        "border: 1px solid #e2e2e2;"
        "border-radius: 8px;"
        );

    m_imageLabel->setText(
        "بدون تصویر"
        );

    headerLayout->addWidget(
        m_imageLabel
        );


    // ---------------------------------------------------------
    // Name + status
    // ---------------------------------------------------------

    auto* identityWidget =
        new QWidget(header);

    identityWidget->setLayoutDirection(
        Qt::RightToLeft
        );

    auto* identityLayout =
        new QVBoxLayout(identityWidget);

    identityLayout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    identityLayout->setSpacing(8);

    identityLayout->setAlignment(
        Qt::AlignTop
        );


    m_nameLabel = new QLabel(
        identityWidget
        );

    m_nameLabel->setWordWrap(true);

    m_nameLabel->setAlignment(
        Qt::AlignVCenter
        );

    m_nameLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 400;"
        "color: #222222;"
        );


    m_statusLabel = new QLabel(
        identityWidget
        );

    m_statusLabel->setAlignment(
        Qt::AlignVCenter
        );

    m_statusLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #555555;"
        );


    identityLayout->addWidget(
        m_nameLabel
        );

    identityLayout->addWidget(
        m_statusLabel
        );

    identityLayout->addStretch();


    headerLayout->addWidget(
        identityWidget,
        1
        );


    layout->addWidget(header);


    // =========================================================
    // Information card
    // =========================================================

    auto* infoCard = new QFrame(panel);

    infoCard->setObjectName(
        "subAssetInfoCard"
        );

    infoCard->setStyleSheet(
        "#subAssetInfoCard {"
        "    background-color: #f8f8f8;"
        "    border: 1px solid #e8e8e8;"
        "    border-radius: 8px;"
        "}"
        );


    auto* infoLayout =
        new QVBoxLayout(infoCard);

    infoLayout->setContentsMargins(
        12,
        10,
        12,
        10
        );

    infoLayout->setSpacing(0);


    // Information title
    auto* infoTitle = new QLabel(
        "اطلاعات زیرتجهیز",
        infoCard
        );

    infoTitle->setLayoutDirection(
        Qt::RightToLeft
        );

    infoTitle->setAlignment(
        Qt::AlignVCenter
        );

    infoTitle->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 400;"
        "color: #222222;"
        "padding-bottom: 8px;"
        );

    infoLayout->addWidget(
        infoTitle
        );


    // Information rows

    infoLayout->addWidget(
        createInfoRow(
            "نوع",
            m_typeLabel,
            infoCard
            )
        );

    infoLayout->addWidget(
        createInfoRow(
            "شماره سریال",
            m_serialNumberLabel,
            infoCard
            )
        );

    infoLayout->addWidget(
        createInfoRow(
            "سازنده",
            m_manufacturerLabel,
            infoCard
            )
        );

    infoLayout->addWidget(
        createInfoRow(
            "مدل",
            m_modelLabel,
            infoCard
            )
        );

    infoLayout->addWidget(
        createInfoRow(
            "تاریخ نصب",
            m_installationDateLabel,
            infoCard
            )
        );

    infoLayout->addWidget(
        createInfoRow(
            "محل نصب",
            m_installationPlaceLabel,
            infoCard
            )
        );

    infoLayout->addWidget(
        createInfoRow(
            "توضیحات",
            m_descriptionLabel,
            infoCard
            )
        );


    layout->addWidget(
        infoCard
        );

    layout->addStretch();


    return panel;
}


QWidget* SubAssetsWidget::createInfoRow(
    const QString& title,
    QLabel*& valueLabel,
    QWidget* parent
    )
{
    auto* row = new QWidget(parent);

    row->setLayoutDirection(
        Qt::RightToLeft
        );


    auto* layout =
        new QHBoxLayout(row);

    layout->setContentsMargins(
        12,
        12,
        12,
        12
        );

    layout->setSpacing(10);


    auto* titleLabel =
        createFieldLabel(
            title,
            row
            );


    valueLabel = new QLabel(row);

    valueLabel->setText("-");

    valueLabel->setWordWrap(true);

    valueLabel->setAlignment(
        Qt::AlignVCenter
        );

    valueLabel->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
        );

    valueLabel->setStyleSheet(
        "color: #333333;"
        "font-size: 13px;"
        );


    layout->addWidget(
        titleLabel
        );

    layout->addWidget(
        valueLabel,
        1
        );


    return row;
}


QLabel* SubAssetsWidget::createFieldLabel(
    const QString& title,
    QWidget* parent
    )
{
    auto* label = new QLabel(
        title,
        parent
        );

    label->setLayoutDirection(
        Qt::RightToLeft
        );

    label->setAlignment(
        Qt::AlignVCenter
        );

    label->setMinimumWidth(
        120
        );

    label->setStyleSheet(
        "color: #777777;"
        "font-size: 13px;"
        "font-weight: 500;"
        );

    return label;
}


// =============================================================
// Asset
// =============================================================
void SubAssetsWidget::setAsset(
    const Asset& asset
    )
{
    m_currentAsset = asset;
    m_assetId = asset.id();

    m_selectedSubAssetId = QUuid();

    clearSubAssetItems();

    const auto& subAssets =
        m_currentAsset.subAssets();

    for (const SubAsset& subAsset : subAssets)
    {
        auto* item =
            new SubAssetItemWidget(
                subAsset,
                nullptr
                );

        m_listLayout->addWidget(item);

        m_items.append(item);


        item->onClicked =
            [this](const QUuid& id)
        {
            showSubAsset(id);
        };


        item->onEditRequested =
            [this](const QUuid& id)
        {
            for (const SubAsset& subAsset :
                 m_currentAsset.subAssets())
            {
                if (subAsset.id() != id)
                    continue;

                SubAssetEditDialog dialog(
                    subAsset.info(),
                    window()
                    );

                if (dialog.exec() != QDialog::Accepted)
                    return;

                const SubAssetInfo updatedInfo =
                    dialog.subAssetInfo();

                emit editSubAssetRequested(
                    m_assetId,
                    id,
                    updatedInfo
                    );

                return;
            }
        };

        item->onDeleteRequested =
            [this](const QUuid& id)
        {
            emit deleteSubAssetRequested(
                m_assetId,
                id
                );
        };
    }

    m_listLayout->addStretch();

    if (!subAssets.isEmpty())
    {
        showSubAsset(
            subAssets.first().id()
            );
    }
}


// =============================================================
// Clear list
// =============================================================

void SubAssetsWidget::clearSubAssetItems()
{
    while (QLayoutItem* child =
           m_listLayout->takeAt(0))
    {
        if (QWidget* widget =
            child->widget())
        {
            widget->deleteLater();
        }

        delete child;
    }

    m_items.clear();
}


// =============================================================
// Show selected subasset
// =============================================================

void SubAssetsWidget::showSubAsset(
    const QUuid& subAssetId
    )
{
    for (const SubAsset& subAsset :
         m_currentAsset.subAssets())
    {
        if (subAsset.id() != subAssetId)
            continue;


        m_selectedSubAssetId =
            subAssetId;


        const SubAssetInfo& info =
            subAsset.info();


        // -----------------------------------------------------
        // Identity
        // -----------------------------------------------------

        m_nameLabel->setText(
            info.name
            );

        m_statusLabel->setText(
            info.status.isEmpty()
                ? "-"
                : info.status
            );


        // -----------------------------------------------------
        // Information
        // -----------------------------------------------------

        m_typeLabel->setText(
            info.type.isEmpty()
                ? "-"
                : info.type
            );

        m_serialNumberLabel->setText(
            info.serialNumber.isEmpty()
                ? "-"
                : info.serialNumber
            );

        m_manufacturerLabel->setText(
            info.manufacturer.isEmpty()
                ? "-"
                : info.manufacturer
            );

        m_modelLabel->setText(
            info.model.isEmpty()
                ? "-"
                : info.model
            );


        m_installationDateLabel->setText(
            info.installationDate.isValid()
                ? info.installationDate.toString(
                      "yyyy/MM/dd"
                      )
                : "-"
            );


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


        // -----------------------------------------------------
        // Image
        // -----------------------------------------------------

        m_imageLabel->clear();


        if (!info.imagePath.isEmpty())
        {
            QPixmap pixmap(
                info.imagePath
                );


            if (!pixmap.isNull())
            {
                m_imageLabel->setPixmap(
                    pixmap.scaled(
                        m_imageLabel->size(),
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation
                        )
                    );
            }
            else
            {
                m_imageLabel->setText(
                    "تصویر نامعتبر است"
                    );
            }
        }
        else
        {
            m_imageLabel->setText(
                "بدون تصویر"
                );
        }


        // -----------------------------------------------------
        // Mark selected item
        // -----------------------------------------------------

        for (QWidget* widget : m_items)
        {
            auto* item =
                static_cast<SubAssetItemWidget*>(widget);

            item->setSelected(
                item->subAssetId() == subAssetId
                );
        }


        break;
    }
}