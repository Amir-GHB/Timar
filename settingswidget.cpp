
#include "settingswidget.h"
#include "datamanager.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QApplication>

SettingsWidget::SettingsWidget(
    DataManager& dataManager,
    QWidget* parent
    )
    : QWidget(parent)
    , m_dataManager(dataManager)
{
    setLayoutDirection(Qt::RightToLeft);

    auto* layout = new QVBoxLayout(this);

    auto* titleLabel = new QLabel(
        QStringLiteral("تنظیمات"),
        this
        );

    auto* descriptionLabel = new QLabel(
        QStringLiteral("ورود و خروج اطلاعات برنامه"),
        this
        );

    m_exportButton = new QPushButton(
        QStringLiteral("خروجی گرفتن از اطلاعات"),
        this
        );

    m_importButton = new QPushButton(
        QStringLiteral("وارد کردن اطلاعات"),
        this
        );

    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel);
    layout->addSpacing(12);
    layout->addWidget(m_exportButton);
    layout->addWidget(m_importButton);
    layout->addStretch();

    connect(
        m_exportButton,
        &QPushButton::clicked,
        this,
        &SettingsWidget::exportData
        );

    connect(
        m_importButton,
        &QPushButton::clicked,
        this,
        &SettingsWidget::importData
        );
}

void SettingsWidget::exportData()
{
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("خروجی گرفتن از اطلاعات"),
        QStringLiteral("timar-backup.json"),
        QStringLiteral("فایل‌های JSON (*.json)")
        );

    if (filePath.isEmpty())
        return;

    QString outputPath = filePath;

    if (QFileInfo(outputPath).suffix().isEmpty())
        outputPath += QStringLiteral(".json");

    if (!m_dataManager.exportData(outputPath))
    {
        QMessageBox::critical(
            this,
            QStringLiteral("خطا در خروجی گرفتن"),
            QStringLiteral(
                "ذخیره اطلاعات در فایل انتخاب‌شده امکان‌پذیر نبود."
                )
            );
        return;
    }

    QMessageBox::information(
        this,
        QStringLiteral("عملیات موفق"),
        QStringLiteral("اطلاعات با موفقیت ذخیره شد.")
        );
}

void SettingsWidget::importData()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("وارد کردن اطلاعات"),
        QString(),
        QStringLiteral("فایل‌های JSON (*.json)")
        );

    if (filePath.isEmpty())
        return;

    const QMessageBox::StandardButton answer =
        QMessageBox::warning(
            this,
            QStringLiteral("وارد کردن اطلاعات"),
            QStringLiteral(
                "با وارد کردن اطلاعات جدید، اطلاعات فعلی برنامه "
                "جایگزین می‌شوند.\nآیا ادامه می‌دهید؟"
                ),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
            );

    if (answer != QMessageBox::Yes)
        return;

    if (!m_dataManager.importData(filePath))
    {
        QMessageBox::critical(
            this,
            QStringLiteral("خطا در وارد کردن اطلاعات"),
            QStringLiteral(
                "وارد کردن فایل امکان‌پذیر نبود. "
                "ممکن است فایل نامعتبر یا ناسازگار باشد."
                )
            );
        return;
    }

    QMessageBox::information(
        this,
        QStringLiteral("عملیات موفق"),
        QStringLiteral("اطلاعات با موفقیت وارد شد.")
        );
}
