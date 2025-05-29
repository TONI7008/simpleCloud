#ifndef BARUPDATER_H
#define BARUPDATER_H

#include <QThread>
#include <QObject>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

class BarUpdater : public QObject {
    Q_OBJECT
public:
    qint64 getValue() const;
    BarUpdater();
    ~BarUpdater(){
        deleteLater();
    }

public slots:
    void increment(qint64 size);
    void setValue(qint64 size);
    void stop(){
        stopped=true;
    }
    void reset();
    void run(){
        while(true){
            if(stopped){
                break;
            }
        }
    }

signals:
    void increased();
    void decreased();

private:
    qint64 m_int;
    bool stopped=false;
};

// Définition des fonctions inline
inline BarUpdater::BarUpdater() : m_int(0) {

}

inline qint64 BarUpdater::getValue() const {
    return m_int;
}

inline void BarUpdater::increment(qint64 size) {
    m_int += size;
    emit increased();
}

inline void BarUpdater::setValue(qint64 size) {
    m_int = size;
    emit increased();
}

inline void BarUpdater::reset() {
    m_int = 0;
}


#endif // BARUPDATER_H
