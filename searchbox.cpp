#include "searchbox.h"

SearchBox::SearchBox(QWidget *parent)
    : QLineEdit(parent)
{
    connect(this, &QLineEdit::textEdited, this, &SearchBox::onTextEdited);
    connect(&m_debounceTimer, &QTimer::timeout, this, &SearchBox::emitDebouncedText);
    m_debounceTimer.setSingleShot(true);
}

void SearchBox::setDebounceDelay(int ms)
{
    m_debounceDelay = qMax(0, ms);
}

void SearchBox::onTextEdited(const QString &text)
{
    m_pendingText = text;
    m_debounceTimer.start(m_debounceDelay);
}

void SearchBox::emitDebouncedText()
{
    emit searchTextChanged(m_pendingText);
}
