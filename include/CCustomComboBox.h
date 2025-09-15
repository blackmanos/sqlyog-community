// Qt-based replacement for the original Win32 custom combo box.
// Provides simple auto-completion based on prefix matching.

#pragma once

#include <QComboBox>
#include <QCompleter>
#include <QStringListModel>

class CCustomComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit CCustomComboBox(QWidget* parent = nullptr);

    // Set the list of items shown in the combo box and used for auto-complete.
    void setItems(const QStringList& items);

private slots:
    void onTextEdited(const QString& text);

private:
    QStringListModel m_model;
    QCompleter* m_completer;
};

