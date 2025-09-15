// Simplified Calendar control implemented using Qt widgets.
// This replaces the extensive Win32 based dialog and its message handlers.

#include "CalendarCtrl.h"

#include <QVBoxLayout>

CalendarCtrl::CalendarCtrl(QWidget* parent)
    : QDialog(parent),
      m_calendar(new QCalendarWidget(this)),
      m_time(new QTimeEdit(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_calendar);
    layout->addWidget(m_time);

    connect(m_calendar, &QCalendarWidget::activated,
            this, &CalendarCtrl::accept);
}

QDateTime CalendarCtrl::selectedDateTime() const
{
    return QDateTime(m_calendar->selectedDate(), m_time->time());
}

void CalendarCtrl::setDateTime(const QDateTime& dt)
{
    m_calendar->setSelectedDate(dt.date());
    m_time->setTime(dt.time());
}

