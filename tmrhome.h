#ifndef TMRHOME_H
#define TMRHOME_H

#include "assetmanager.h"
#include "datamanager.h"
#include "maintenancemanager.h"
#include "storagemanager.h"
#include "subassetswidget.h"

#include <QMainWindow>
#include <qscrollarea.h>
#include <qsplitter.h>
#include <QStackedWidget>

class QTreeWidget;
class NavigationPanel;
class QFrame;
class QLabel;
class QPushButton;
class QStackedWidget;
class AssetOverviewWidget;
class AssetSection;
class TimelineWidget;
class ReportsSection;
class StorageSection;

QT_BEGIN_NAMESPACE
namespace Ui {
class tmrhome;
}
QT_END_NAMESPACE

class Tmrhome : public QMainWindow
{
    Q_OBJECT

public:
    explicit Tmrhome(QWidget *parent = nullptr);
    ~Tmrhome() override;

private:
    void setupUI();

    NavigationPanel* createNavPanel(QWidget* parent = nullptr);
    QFrame* createMaintContents(QWidget* parent);
    QFrame* createTreePanel(QWidget* parent);
    void setupAssetTree();
    QWidget* createAssetContentPanel(QWidget* parent);
    void updateAssetContent(const Asset& asset);
    void refreshApplicationData();

    void createDemoAssets();

    QWidget* createAssetHeader(QWidget* parent);
    QWidget* createAssetTabs(QWidget* parent);
    QStackedWidget* createAssetPages(QWidget* parent);

    QWidget* createEventsPage(QWidget* parent);
    QWidget* createSchedulesContentPanel();
    QWidget* createPartsPage(QWidget* parent);

    void updateEventsForAsset(const Asset& asset);
    void refreshCurrentAssetEvents();

    QScrollArea* m_schedulesScrollArea = nullptr;
    QVBoxLayout* m_schedulesLayout = nullptr;
    void updateSchedulesForAsset(const Asset& asset);
    void refreshCurrentAssetSchedules();

    void refreshPartsPage(const Asset& asset);
    QWidget* createPartRow(
        const AssetPart& assetPart,
        const Asset& asset
        );

private:
    Ui::tmrhome *ui;

    AssetManager m_assetManager;
    MaintenanceManager m_maintenanceManager;
    DataManager m_dataManager;
    StorageManager m_storageManager;

    QSplitter *m_mainSplitter = nullptr;
    QFrame* treePanel = nullptr;
    QTreeWidget* assetTree = nullptr;
    QStackedWidget *m_mainContentStack = nullptr;
    NavigationPanel *m_navigationPanel = nullptr;

    QLabel* m_assetNameLabel = nullptr;
    QLabel* m_assetSerialLabel = nullptr;
    QLabel* m_assetImageLabel = nullptr;

    QStackedWidget* m_assetContentStack = nullptr;
    QVBoxLayout* eventsLayout = nullptr;

    QPushButton* m_overviewTab = nullptr;
    QPushButton* m_historyTab = nullptr;
    QPushButton* m_subAssetsTab = nullptr;
    QPushButton* m_eventsTab = nullptr;
    QPushButton* m_schedulesTab = nullptr;
    QPushButton* m_partsTab = nullptr;

    AssetOverviewWidget* m_overviewWidget = nullptr;
    SubAssetsWidget* m_subAssetsWidget = nullptr;

    AssetSection* m_assetSection = nullptr;
    TimelineWidget* m_timelineWidget = nullptr;
    ReportsSection* m_reportssection = nullptr;
    StorageSection* m_storageSection = nullptr;

    QVBoxLayout* m_partsLayout = nullptr;
    QPushButton* m_addPartButton = nullptr;

private slots:
    void openCreateEventDialog();
    void createScheduleDialog();

};
#endif // TMRHOME_H
