// Qt based implementation of the legacy CCustomComboBox control.
// The previous version used Win32 message handling and subclassing to
// implement auto-completion.  This rewrite subclasses QComboBox and uses
// standard Qt facilities to provide similar behaviour.

#include "CCustomComboBox.h"

#include <QLineEdit>

CCustomComboBox::CCustomComboBox(QWidget* parent)
    : QComboBox(parent), m_completer(new QCompleter(this))
{
    setEditable(true);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(m_completer);

    connect(lineEdit(), &QLineEdit::textEdited,
            this, &CCustomComboBox::onTextEdited);
}

void CCustomComboBox::setItems(const QStringList& items)
{
    clear();
    addItems(items);
    m_model.setStringList(items);
    m_completer->setModel(&m_model);
}

void CCustomComboBox::onTextEdited(const QString& text)
{
    int idx = findText(text, Qt::MatchStartsWith);
    if (idx >= 0) {
        QString completion = itemText(idx);
        lineEdit()->setText(completion);
        lineEdit()->setSelection(text.length(), completion.length() - text.length());
    }
}

