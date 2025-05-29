#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QLineEdit>
#include <QTimer>

class SearchBox : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchBox(QWidget *parent = nullptr);
    void setDebounceDelay(int ms);

signals:
    void searchTextChanged(const QString &text);

private slots:
    void onTextEdited(const QString &text);
    void emitDebouncedText();

private:
    QTimer m_debounceTimer;
    QString m_pendingText;
    int m_debounceDelay = 500; // default 300ms
};

#endif // SEARCHBOX_H
