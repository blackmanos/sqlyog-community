// Simplified Qt-based implementation of the legacy ButtonDropDown control.
// The previous Windows specific code relied heavily on HWND and custom
// message handling.  This version uses QToolButton with an attached QMenu
// and emits a signal whenever an item is triggered.

#include "ButtonDropDown.h"

#include <QAction>

ButtonDropDown::ButtonDropDown(QWidget* parent)
    : QToolButton(parent), m_menu(new QMenu(this))
{
    setMenu(m_menu);
    setPopupMode(QToolButton::InstantPopup);

    connect(m_menu, &QMenu::triggered,
            this, &ButtonDropDown::onActionTriggered);
}

void ButtonDropDown::setItems(const QStringList& items)
{
    m_menu->clear();
    for (int i = 0; i < items.size(); ++i) {
        QAction* act = m_menu->addAction(items.at(i));
        act->setData(i);
    }
}

void ButtonDropDown::onActionTriggered(QAction* act)
{
    bool ok = false;
    int index = act->data().toInt(&ok);
    if (ok)
        emit itemSelected(index);
}

bool RegisterButtonDropDown(HINSTANCE)
{
    // The Qt version does not require explicit registration.
    return true;
}

