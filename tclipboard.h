#ifndef TCLIPBOARD_H
#define TCLIPBOARD_H

#include <QObject>
#include "tcloudelt.h"


class TClipBoard : public QObject
{
    Q_OBJECT
public:
    enum transactionType{
        Copy,
        Cut,
        Restore
    };
    static TClipBoard *instance();
    static void Init();
    static void cleanUp();

    // Additional helpful methods
    void append(eltCore item);
    bool isEmpty() const { return m_list.isEmpty(); }
    int size() const { return m_list.size(); }

    QList<eltCore> items() const { return m_list; }
    void clear();


    transactionType type() const;
    void setType(transactionType newType);

private:
    explicit TClipBoard(QObject *parent = nullptr);
    static TClipBoard* clipBoard;
    QList<eltCore> m_list;
    transactionType m_type;

    bool m_isUsed;
};

#endif // TCLIPBOARD_H
