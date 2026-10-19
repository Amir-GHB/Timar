#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include "assetmanager.h"
#include "maintenanceevent.h"

#include <QDate>
#include <QList>
#include <QUuid>
#include <QWidget>
#include <qpushbutton.h>

class QCalendarWidget;
class QScrollArea;
class QVBoxLayout;
class QLabel;
class PersianCalendarWidget;

class MaintenanceManager;

class TimelineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineWidget(
        MaintenanceManager* maintenanceManager,
        AssetManager* assetManager,
        QWidget* parent = nullptr
        );

    void setSelectedAsset(
        const QUuid& assetId,
        const QString& assetName,
        const QString& assetType,
        const QString& serialNumber,
        const QString& imagePath
        );

public slots:
    void refresh();

private:
    void setupUI();
    void setupTimeline();
    void setupRightPanel();

    void refreshTimeline();
    void refreshRightPanel();

    void clearTimeline();
    void updateSelectedEventInfo();
    void selectEvent(const MaintenanceEventData& event);
    void clearSelectedEvent();

    QList<MaintenanceEventData> eventsForDate(
        const QDate& date
        ) const;

    QString dayName(
        const QDate& date
        ) const;

    QString formatPersianDate(
        const QDate& date
        ) const;

    QString statusText(
        MaintenanceStatus status
        ) const;

    QString statusIcon(
        MaintenanceStatus status
        ) const;

    QString statusColor(
        MaintenanceStatus status
        ) const;

    QString statusBackgroundColor(
        MaintenanceStatus status
        ) const;

    QWidget* createEventWidget(
        const MaintenanceEventData& event
        );

    QWidget* createStatusLegend();

    void refreshCalendar();
    //date-time line
private:

    void loadInitialTimeline();

    void prependDays(int count);

    void appendDays(int count);

    void addDay(
        const QDate& date,
        bool prepend = false
        );

    QWidget* createDayWidget(
        const QDate& date
        );

    QWidget* createEmptyDayWidget(
        const QDate& date
        );

    void checkScrollPosition();

    void scrollToDate(
        const QDate& date
        );

    QDate findNextEventDate() const;

    void updateNextEventButton();

    void positionNextEventButton();

    void showCalendarDate(const QDate& date);
    void buildTimelineAroundDate(const QDate& centerDate);

protected:
    void resizeEvent(
        QResizeEvent* event
        ) override;

    bool eventFilter(
        QObject* watched,
        QEvent* event
        ) override;

    void showEvent(QShowEvent* event) override;

private:
    MaintenanceManager* m_maintenanceManager = nullptr;
    AssetManager* m_assetManager = nullptr;

    QScrollArea* m_scrollArea = nullptr;
    QVBoxLayout* m_timelineLayout = nullptr;

    PersianCalendarWidget* m_calendarWidget = nullptr;

    QLabel* m_assetNameLabel = nullptr;
    QLabel* m_assetTypeLabel = nullptr;
    QLabel* m_assetSerialLabel = nullptr;
    QLabel* m_assetImageLabel = nullptr;

    QLabel* m_totalEventsLabel = nullptr;
    QLabel* m_completedEventsLabel = nullptr;
    QLabel* m_overdueEventsLabel = nullptr;


    QUuid m_selectedEventId;

    QDate m_loadedStartDate;
    QDate m_loadedEndDate;

    QPushButton* m_nextEventButton = nullptr;

    bool m_showEmptyDays = false;
    QWidget* m_loadingIndicator = nullptr;
    QLabel* m_loadingLabel = nullptr;
    QTimer* m_loadingAnimationTimer = nullptr;

    bool m_loadingMoreDays = false;
    int m_loadingAnimationFrame = 0;

    void showLoadingIndicator(bool atTop);
    void hideLoadingIndicator();

    void loadMoreDays(bool towardTop);
    bool m_loadingTimeline = false;

    void resetTimeline();
    void scrollToDateTop(const QDate& date);

    QWidget* m_topLoadZone = nullptr;
    QWidget* m_bottomLoadZone = nullptr;

    bool m_loadingAtTop = false;

    QWidget* m_scrollBottomSpacer = nullptr;


};

#endif // TIMELINEWIDGET_H