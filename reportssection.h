#ifndef REPORTSSECTION_H
#define REPORTSSECTION_H

#include "maintenanceevent.h"
#include "maintenancereporttypes.h"
#include "persiandateedit.h"

#include <QCheckBox>
#include <QList>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class MaintenanceManager;

class QScrollArea;
class QVBoxLayout;
class QComboBox;
class QPushButton;
class QDateEdit;
class QLabel;
class QFrame;

class ReportsSection : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsSection(
        MaintenanceManager& maintenanceManager,
        QWidget* parent = nullptr
        );

    void refreshAssets();

private:
    QWidget* createResultsPanel();
    QWidget* createSidePanel();

    void populateReport();
    void clearResults();

    MaintenanceEventFilter buildFilter() const;

    void updateSummary();


    void exportReport();
    bool exportCsv(
        const QString& filePath,
        const QList<MaintenanceEventData>& events
        );

    bool exportPdf(
        const QString& filePath,
        const QList<MaintenanceEventData>& events
        );

    void setReportDateRange(
        const QDate& fromDate,
        const QDate& toDate
        );

    void populateAssetFilter();

private slots:
    void setCurrentWeekRange();
    void setCurrentMonthRange();
    void setLastSixMonthsRange();
    void setCurrentYearRange();

private:
    MaintenanceManager& m_maintenanceManager;

    QScrollArea* m_resultsScrollArea = nullptr;
    QWidget* m_resultsContainer = nullptr;
    QVBoxLayout* m_resultsLayout = nullptr;

    QComboBox* m_statusCombo = nullptr;
    QCheckBox* m_allAssetsCheckBox = nullptr;
    QScrollArea* m_assetScrollArea = nullptr;
    QWidget* m_assetListWidget = nullptr;
    QVBoxLayout* m_assetListLayout = nullptr;

    QList<QCheckBox*> m_assetChecks;

    PersianDateEdit* m_fromDateEdit = nullptr;
    PersianDateEdit* m_toDateEdit = nullptr;

    QPushButton* m_generateButton = nullptr;

    QComboBox* m_exportFormatCombo = nullptr;
    QPushButton* m_exportButton = nullptr;

    QWidget* m_summaryContainer = nullptr;

    QLabel* m_scheduledCount = nullptr;
    QLabel* m_completedCount = nullptr;
    QLabel* m_cancelledCount = nullptr;
    QLabel* m_overdueCount = nullptr;
    QLabel* m_missedCount = nullptr;

};

#endif // REPORTSSECTION_H