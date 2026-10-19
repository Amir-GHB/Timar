
#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

class DataManager;
class QPushButton;
class QCheckBox;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(
        DataManager& dataManager,
        QWidget* parent = nullptr
        );

private slots:
    void exportData();
    void importData();

private:
    DataManager& m_dataManager;

    QPushButton* m_exportButton = nullptr;
    QPushButton* m_importButton = nullptr;
};

#endif // SETTINGSWIDGET_H
