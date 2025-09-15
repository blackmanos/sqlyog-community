// Qt-based calendar dialog used for selecting date and time values.

#pragma once

#include <QDialog>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QDateTime>

class CalendarCtrl : public QDialog
{
    Q_OBJECT

public:
    explicit CalendarCtrl(QWidget* parent = nullptr);

    QDateTime selectedDateTime() const;
    void setDateTime(const QDateTime& dt);

private:
    QCalendarWidget* m_calendar;
    QTimeEdit* m_time;
};

