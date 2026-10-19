#include "storagesection.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QVBoxLayout>
#include <qdebug.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qspinbox.h>

StorageSection::StorageSection(
    StorageManager& storageManager,
    QWidget* parent
    )
    : QWidget(parent)
    , m_storageManager(storageManager)
{

    qDebug() << "STORAGE: constructor entered";
    setObjectName("storageSection");
    setLayoutDirection(Qt::RightToLeft);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    auto* title = new QLabel("انبار قطعات", this);
    title->setStyleSheet(
        "font-size: 18px;"
        "font-weight: 700;"
        "color: #0f172a;"
        );

    auto* addButton = new QPushButton("افزودن قطعه", this);
    addButton->setMinimumHeight(36);
    addButton->setFocusPolicy(Qt::NoFocus);

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(addButton);

    mainLayout->addLayout(headerLayout);

    // Parts area
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* container = new QWidget();
    container->setObjectName("storagePartsContainer");

    m_partsLayout = new QVBoxLayout(container);
    m_partsLayout->setContentsMargins(0, 0, 0, 0);
    m_partsLayout->setSpacing(8);

    m_partsLayout->addStretch();

    scrollArea->setWidget(container);

    mainLayout->addWidget(scrollArea);

    connect(
        addButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            StoragePart part;

            if (showPartDialog(part, false))
            {
                m_storageManager.addPart(part);
                refreshParts();
            }
        }
        );

    connect(
        &m_storageManager,
        &StorageManager::partsUpdated,
        this,
        [this]()
        {
            refreshParts();
        }
        );

    qDebug() << "STORAGE: before refreshParts";

    refreshParts();

    qDebug() << "STORAGE: after refreshParts";
}

QWidget* StorageSection::createPartRow(const StoragePart& part)
{
    auto* frame = new QFrame();
    frame->setObjectName("storagePartRow");
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Plain);
    frame->setMinimumHeight(80);

    frame->setStyleSheet(
        "QFrame#storagePartRow {"
        "background-color: #ffffff;"
        "border: 1px solid #e2e8f0;"
        "border-radius: 8px;"
        "}"
        );

    auto* layout = new QHBoxLayout(frame);
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(10);

    auto* infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(3);

    auto* nameLabel = new QLabel(part.name, frame);
    nameLabel->setStyleSheet(
        "font-weight: 600;"
        "color: #0f172a;"
        );

    auto* detailsLabel =
        new QLabel(
            QString("%1%2")
                .arg(part.partNumber)
                .arg(
                    part.description.isEmpty()
                        ? ""
                        : QString("  •  %1").arg(part.description)
                    ),
            frame
            );

    detailsLabel->setStyleSheet(
        "color: #64748b;"
        );

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(detailsLabel);

    layout->addLayout(infoLayout);
    layout->addStretch();

    auto* decreaseButton =
        new QPushButton("-", frame);

    auto* quantityLabel =
        new QLabel(
            QString::number(part.quantity),
            frame
            );

    quantityLabel->setMinimumWidth(30);
    quantityLabel->setAlignment(Qt::AlignCenter);
    quantityLabel->setStyleSheet(
        "font-weight: 600;"
        "color: #0f172a;"
        );

    auto* increaseButton =
        new QPushButton("+", frame);

    auto* editButton =
        new QPushButton("ویرایش", frame);

    auto* deleteButton =
        new QPushButton("حذف", frame);

    decreaseButton->setEnabled(part.quantity > 0);

    const QList<QPushButton*> buttons =
        {
            decreaseButton,
            increaseButton,
            editButton,
            deleteButton
        };

    for (auto* button : buttons)
    {
        button->setMinimumHeight(30);
        button->setFocusPolicy(Qt::NoFocus);
    }

    layout->addWidget(decreaseButton);
    layout->addWidget(quantityLabel);
    layout->addWidget(increaseButton);
    layout->addSpacing(8);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);

    connect(
        increaseButton,
        &QPushButton::clicked,
        this,
        [this, part]()
        {
            m_storageManager.increaseQuantity(part.id);
            refreshParts();
        }
        );

    connect(
        decreaseButton,
        &QPushButton::clicked,
        this,
        [this, part]()
        {
            m_storageManager.decreaseQuantity(part.id);
            refreshParts();
        }
        );

    connect(
        deleteButton,
        &QPushButton::clicked,
        this,
        [this, part]()
        {
            m_storageManager.removePart(part.id);
            refreshParts();
        }
        );

    connect(
        editButton,
        &QPushButton::clicked,
        this,
        [this, part]()
        {
            StoragePart editedPart = part;

            if (showPartDialog(editedPart, true))
            {
                m_storageManager.updatePart(editedPart);
                refreshParts();
            }
        }
        );

    return frame;
}

void StorageSection::refreshParts()
{
    qDebug() << "STORAGE: refreshParts entered";
    while (m_partsLayout->count() > 1)
    {
        QLayoutItem* item =
            m_partsLayout->takeAt(0);

        if (item->widget())
            delete item->widget();

        delete item;
    }

    for (const auto& part : m_storageManager.parts())
    {
        auto* row = createPartRow(part);

        m_partsLayout->insertWidget(
            m_partsLayout->count() - 1,
            row
            );
    }
}

bool StorageSection::showPartDialog(
    StoragePart& part,
    bool editing
    )
{
    QDialog dialog(this);

    dialog.setWindowTitle(
        editing ? "ویرایش قطعه" : "افزودن قطعه"
        );

    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setMinimumWidth(360);

    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);

    auto* nameLabel =
        new QLabel("نام قطعه", &dialog);

    auto* nameEdit =
        new QLineEdit(&dialog);

    auto* numberLabel =
        new QLabel("شماره قطعه", &dialog);

    auto* numberEdit =
        new QLineEdit(&dialog);

    auto* descriptionLabel =
        new QLabel("توضیحات", &dialog);

    auto* descriptionEdit =
        new QLineEdit(&dialog);

    auto* quantityLabel =
        new QLabel("تعداد", &dialog);

    auto* quantitySpinBox =
        new QSpinBox(&dialog);

    quantitySpinBox->setRange(0, 999999);

    // If editing, populate the existing values.
    if (editing)
    {
        nameEdit->setText(part.name);
        numberEdit->setText(part.partNumber);
        descriptionEdit->setText(part.description);
        quantitySpinBox->setValue(part.quantity);
    }
    else
    {
        quantitySpinBox->setValue(1);
    }

    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);

    layout->addWidget(numberLabel);
    layout->addWidget(numberEdit);

    layout->addWidget(descriptionLabel);
    layout->addWidget(descriptionEdit);

    layout->addWidget(quantityLabel);
    layout->addWidget(quantitySpinBox);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 8, 0, 0);

    auto* cancelButton =
        new QPushButton("لغو", &dialog);

    auto* saveButton =
        new QPushButton(
            editing ? "ذخیره" : "افزودن",
            &dialog
            );

    saveButton->setDefault(true);

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);

    layout->addLayout(buttonLayout);

    connect(
        cancelButton,
        &QPushButton::clicked,
        &dialog,
        &QDialog::reject
        );

    connect(
        saveButton,
        &QPushButton::clicked,
        &dialog,
        [&]()
        {
            if (nameEdit->text().trimmed().isEmpty())
            {
                nameEdit->setFocus();
                return;
            }

            if (!editing)
                part.id = QUuid::createUuid();

            part.name =
                nameEdit->text().trimmed();

            part.partNumber =
                numberEdit->text().trimmed();

            part.description =
                descriptionEdit->text().trimmed();

            part.quantity =
                quantitySpinBox->value();

            dialog.accept();
        }
        );

    return dialog.exec() == QDialog::Accepted;
}