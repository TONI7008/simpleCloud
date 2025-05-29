#ifndef TLINEEDIT_HPP
#define TLINEEDIT_HPP

#include <QLineEdit>
#include <QFocusEvent>
#include <QTimer>

class TLineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit TLineEdit(QWidget *parent = nullptr)
        : QLineEdit(parent)
    {
        connect(this, &QLineEdit::textEdited, this, &TLineEdit::onTextEdited);
        connect(&m_debounceTimer, &QTimer::timeout, this, &TLineEdit::emitDebouncedText);
        m_debounceTimer.setSingleShot(true);
    }

    void setDebounceDelay(int ms) {
        m_debounceDelay = qMax(0, ms);
    }

signals:
    void focusEntered();
    void focusLeave();
    void searchTextChanged(const QString &text);

protected:
    void focusInEvent(QFocusEvent *event) override {
        QLineEdit::focusInEvent(event);
        emit focusEntered();
    }

    void focusOutEvent(QFocusEvent *event) override {
        QLineEdit::focusOutEvent(event);
        emit focusLeave();
    }

private slots:
    void onTextEdited(const QString &text) {
        m_pendingText = text;
        m_debounceTimer.start(m_debounceDelay);
    }

    void emitDebouncedText() {
        emit searchTextChanged(m_pendingText);
    }

private:
    QTimer m_debounceTimer;
    QString m_pendingText;
    int m_debounceDelay = 500; // default 500ms
};

#endif // TLINEEDIT_HPP
