#ifndef TWORKER_H
#define TWORKER_H

#include <QObject>
#include <QThread>
#include <QTimer>

class TWorker : public QObject
{
    Q_OBJECT
public:
    explicit TWorker(QObject *parent = nullptr);
    ~TWorker();

    enum Priority{
        Low,
        Normal,
        High
    };

    void build();
    void stop();
    void restart();
    bool isStopped();
    void start(TWorker::Priority defaultValue=Normal);
    void setPriority(TWorker::Priority);
    TWorker::Priority priority();
signals:
    void finished();
protected:
    virtual void work();

private:
    const short sleepL=500;
    const short sleepN=250;
    const short sleepH=100;

    bool _init=false;
    bool _stop=false;


    QTimer* m_timer;
    short m_sleeptime;
    TWorker::Priority m_priority;
    QThread *parentThread;

    void setSleepTime(short);

};

#endif // TWORKER_H
