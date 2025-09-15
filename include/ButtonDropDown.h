// Qt based replacement for the old ButtonDropDown Win32 control.
// Provides a simple drop-down button backed by QToolButton and QMenu.

#pragma once

#include <QToolButton>
#include <QMenu>

#ifdef _WIN32
#include <windows.h>
#else
using HINSTANCE = void*;
#endif

class ButtonDropDown : public QToolButton
{
    Q_OBJECT

public:
    explicit ButtonDropDown(QWidget* parent = nullptr);

    // Populate the drop-down list with items.
    void setItems(const QStringList& items);

signals:
    // Emitted when the user selects an item from the menu.
    void itemSelected(int index);

private slots:
    void onActionTriggered(QAction* act);

private:
    QMenu* m_menu;
};

// Compatibility helper. The Qt implementation does not require
// explicit registration but the old code expects this function.
bool RegisterButtonDropDown(HINSTANCE hInstance);

