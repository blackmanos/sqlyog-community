#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

class AppSettings : public QObject
{
    Q_OBJECT
public:
    static AppSettings &instance();

    QString toolIconSize() const;
    void setToolIconSize(const QString &size);

private:
    explicit AppSettings(QObject *parent = nullptr);
    QSettings m_settings;
};
