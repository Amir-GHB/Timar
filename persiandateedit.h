#pragma once

#include <QDateEdit>

class PersianCalendarWidget;

class PersianDateEdit : public QDateEdit
{
    Q_OBJECT

public:
    explicit PersianDateEdit(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    PersianCalendarWidget* m_calendar = nullptr;
};