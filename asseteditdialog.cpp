#include "asseteditdialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

AssetEditDialog::AssetEditDialog(
    const AssetInfo& info,
    QWidget* parent
    )
    : QDialog(parent),
    m_info(info)
{
    setupUI();
}

void AssetEditDialog::setupUI()
{
    setWindowTitle("ویرایش تجهیز");
    setMinimumWidth(500);

    setLayoutDirection(Qt::RightToLeft);

    auto* mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        20, 20, 20, 20
        );

    mainLayout->setSpacing(16);

    auto* formLayout =
        new QFormLayout;

    formLayout->setSpacing(12);
    formLayout->setLabelAlignment(
        Qt::AlignRight
        );

    m_nameEdit =
        new QLineEdit(this);

    m_typeEdit =
        new QLineEdit(this);

    m_serialEdit =
        new QLineEdit(this);

    m_manufacturerEdit =
        new QLineEdit(this);

    m_modelEdit =
        new QLineEdit(this);

    m_installationDateEdit =
        new PersianDateEdit(this);


    m_installationPlaceEdit =
        new QLineEdit(this);

    m_descriptionEdit =
        new QTextEdit(this);

    m_descriptionEdit->setMinimumHeight(100);

    // Existing values
    m_nameEdit->setText(m_info.name);
    m_typeEdit->setText(m_info.assetType);
    m_serialEdit->setText(m_info.serialNumber);

    m_manufacturerEdit->setText(
        m_info.manufacturer
        );

    m_modelEdit->setText(
        m_info.model
        );

    m_installationPlaceEdit->setText(
        m_info.installationPlace
        );

    m_descriptionEdit->setPlainText(
        m_info.description
        );

    if (m_info.installationDate.isValid())
    {
        m_installationDateEdit->setDate(
            m_info.installationDate
            );
    }
    else
    {
        m_installationDateEdit->setDate(
            QDate::currentDate()
            );
    }

    /*
     * Required field error labels.
     */

    auto createFieldWidget =
        [this](
            QLineEdit* field,
            QLabel*& errorLabel
            )
    {
        auto* widget =
            new QWidget(this);

        auto* layout =
            new QHBoxLayout(widget);

        layout->setContentsMargins(
            0, 0, 0, 0
            );

        layout->setSpacing(6);

        errorLabel =
            new QLabel("!", widget);

        errorLabel->setStyleSheet(
            "QLabel {"
            "color: #e74c3c;"
            "font-weight: bold;"
            "font-size: 18px;"
            "}"
            );

        errorLabel->setVisible(false);

        layout->addWidget(
            field,
            1
            );

        layout->addWidget(
            errorLabel
            );

        return widget;
    };

    formLayout->addRow(
        "نام تجهیز",
        createFieldWidget(
            m_nameEdit,
            m_nameErrorLabel
            )
        );

    formLayout->addRow(
        "نوع تجهیز",
        createFieldWidget(
            m_typeEdit,
            m_typeErrorLabel
            )
        );

    formLayout->addRow(
        "شماره سریال",
        createFieldWidget(
            m_serialEdit,
            m_serialErrorLabel
            )
        );

    formLayout->addRow(
        "سازنده",
        m_manufacturerEdit
        );

    formLayout->addRow(
        "مدل",
        m_modelEdit
        );

    formLayout->addRow(
        "تاریخ نصب",
        m_installationDateEdit
        );

    formLayout->addRow(
        "محل نصب",
        createFieldWidget(
            m_installationPlaceEdit,
            m_installationPlaceErrorLabel
            )
        );

    formLayout->addRow(
        "توضیحات",
        m_descriptionEdit
        );

    mainLayout->addLayout(
        formLayout
        );

    /*
     * Image selector.
     */

    auto* imageWidget =
        new QWidget(this);

    auto* imageLayout =
        new QHBoxLayout(imageWidget);

    imageLayout->setContentsMargins(
        0, 0, 0, 0
        );

    imageLayout->setSpacing(8);

    m_imagePathEdit =
        new QLineEdit(imageWidget);

    m_imagePathEdit->setReadOnly(true);

    m_imagePathEdit->setText(
        m_info.imagePath
        );

    auto* browseButton =
        new QPushButton(
            "انتخاب تصویر...",
            imageWidget
            );

    imageLayout->addWidget(
        m_imagePathEdit,
        1
        );

    imageLayout->addWidget(
        browseButton
        );

    connect(
        browseButton,
        &QPushButton::clicked,
        this,
        &AssetEditDialog::browseImage
        );

    formLayout->addRow(
        "تصویر تجهیز",
        imageWidget
        );

    /*
     * Dialog buttons.
     */

    auto* buttons =
        new QDialogButtonBox(
            QDialogButtonBox::Ok |
                QDialogButtonBox::Cancel,
            this
            );

    buttons->button(
               QDialogButtonBox::Ok
               )->setText("ذخیره");

    buttons->button(
               QDialogButtonBox::Cancel
               )->setText("انصراف");

    /*
     * Validation is completely internal
     * to this dialog.
     */
    connect(
        buttons,
        &QDialogButtonBox::accepted,
        this,
        [this]()
        {
            if (validateFields())
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

    /*
     * Remove validation indicators immediately
     * when the user fills a required field.
     */

    connect(
        m_nameEdit,
        &QLineEdit::textChanged,
        this,
        [this](const QString& text)
        {
            const bool error =
                text.trimmed().isEmpty();

            m_nameErrorLabel->setVisible(error);

            setFieldError(
                m_nameEdit,
                error
                );
        }
        );

    connect(
        m_typeEdit,
        &QLineEdit::textChanged,
        this,
        [this](const QString& text)
        {
            const bool error =
                text.trimmed().isEmpty();

            m_typeErrorLabel->setVisible(error);

            setFieldError(
                m_typeEdit,
                error
                );
        }
        );

    connect(
        m_serialEdit,
        &QLineEdit::textChanged,
        this,
        [this](const QString& text)
        {
            const bool error =
                text.trimmed().isEmpty();

            m_serialErrorLabel->setVisible(error);

            setFieldError(
                m_serialEdit,
                error
                );
        }
        );

    connect(
        m_installationPlaceEdit,
        &QLineEdit::textChanged,
        this,
        [this](const QString& text)
        {
            const bool error =
                text.trimmed().isEmpty();

            m_installationPlaceErrorLabel
                ->setVisible(error);

            setFieldError(
                m_installationPlaceEdit,
                error
                );
        }
        );
}

bool AssetEditDialog::validateFields()
{
    const bool nameError =
        m_nameEdit->text()
            .trimmed()
            .isEmpty();

    const bool typeError =
        m_typeEdit->text()
            .trimmed()
            .isEmpty();

    const bool serialError =
        m_serialEdit->text()
            .trimmed()
            .isEmpty();

    const bool installationPlaceError =
        m_installationPlaceEdit->text()
            .trimmed()
            .isEmpty();

    m_nameErrorLabel->setVisible(
        nameError
        );

    m_typeErrorLabel->setVisible(
        typeError
        );

    m_serialErrorLabel->setVisible(
        serialError
        );

    m_installationPlaceErrorLabel->setVisible(
        installationPlaceError
        );

    setFieldError(
        m_nameEdit,
        nameError
        );

    setFieldError(
        m_typeEdit,
        typeError
        );

    setFieldError(
        m_serialEdit,
        serialError
        );

    setFieldError(
        m_installationPlaceEdit,
        installationPlaceError
        );

    if (nameError)
    {
        m_nameEdit->setFocus();
    }
    else if (typeError)
    {
        m_typeEdit->setFocus();
    }
    else if (serialError)
    {
        m_serialEdit->setFocus();
    }
    else if (installationPlaceError)
    {
        m_installationPlaceEdit->setFocus();
    }

    return !nameError &&
           !typeError &&
           !serialError &&
           !installationPlaceError;
}

AssetInfo AssetEditDialog::assetInfo() const
{
    AssetInfo info = m_info;

    info.name =
        m_nameEdit->text();

    info.assetType =
        m_typeEdit->text();

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

    info.description =
        m_descriptionEdit->toPlainText();

    info.imagePath =
        m_imagePathEdit->text();

    return info;
}

void AssetEditDialog::browseImage()
{
    const QString filePath =
        QFileDialog::getOpenFileName(
            this,
            "انتخاب تصویر تجهیز",
            QString(),
            "Images (*.png *.jpg *.jpeg *.bmp *.gif *.webp *.tif *.tiff);;All Files (*)"
            );

    if (filePath.isEmpty())
        return;

    m_imagePathEdit->setText(
        filePath
        );
}

void AssetEditDialog::setFieldError(
    QLineEdit* field,
    bool error
    )
{
    if (error)
    {
        field->setStyleSheet(
            "QLineEdit {"
            "border: 1px solid #e74c3c;"
            "}"
            );

        field->setToolTip(
            "این فیلد الزامی است."
            );
    }
    else
    {
        field->setStyleSheet(
            {}
            );

        field->setToolTip(
            {}
            );
    }
}