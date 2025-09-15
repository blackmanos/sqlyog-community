#include "AppContext.h"

AppContext &AppContext::instance()
{
    static AppContext ctx;
    return ctx;
}

AppContext::AppContext() = default;

