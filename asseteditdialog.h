#ifndef ASSETEDITDIALOG_H
#define ASSETEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QLabel>

#include "asset.h"
#include "persiandateedit.h"

class AssetEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AssetEditDialog(
        const AssetInfo& info,
        QWidget* parent = nullptr
        );

    AssetInfo assetInfo() const;

    void browseImage();

private:
    bool validateFields();

    void setFieldError(
        QLineEdit* field,
        bool error
        );

    QLabel* m_nameErrorLabel = nullptr;
    QLabel* m_typeErrorLabel = nullptr;
    QLabel* m_serialErrorLabel = nullptr;
    QLabel* m_installationPlaceErrorLabel = nullptr;

private:
    void setupUI();

    QLineEdit* m_nameEdit = nullptr;
    QLineEdit* m_typeEdit = nullptr;
    QLineEdit* m_serialEdit = nullptr;
    QLineEdit* m_manufacturerEdit = nullptr;
    QLineEdit* m_modelEdit = nullptr;
    PersianDateEdit* m_installationDateEdit = nullptr;
    QLineEdit* m_installationPlaceEdit = nullptr;
    QTextEdit* m_descriptionEdit = nullptr;
    QLineEdit* m_imagePathEdit = nullptr;

    AssetInfo m_info;
};

#endif