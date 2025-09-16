#include "AppSettings.h"

AppSettings &AppSettings::instance()
{
    static AppSettings inst;
    return inst;
}

AppSettings::AppSettings(QObject *parent)
    : QObject(parent),
      m_settings("SQLyog", "SQLyog")
{
}

QString AppSettings::toolIconSize() const
{
    return m_settings.value("ToolIconSize", QStringLiteral("Large")).toString();
}

void AppSettings::setToolIconSize(const QString &size)
{
    m_settings.setValue("ToolIconSize", size);
}
