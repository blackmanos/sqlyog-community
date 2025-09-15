#pragma once

#include <QMainWindow>

// Basic main window for the Qt based application.  In the original code base
// the window management relied heavily on WinAPI.  Using QMainWindow allows us
// to run the application in a cross‑platform manner.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
};

