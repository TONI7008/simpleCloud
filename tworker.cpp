#include "tworker.h"

TWorker::TWorker(QObject *parent)
    : QObject{parent},
    m_sleeptime(sleepN),
    m_priority(Normal)
{
    _init=true;
}

TWorker::~TWorker()
{
    delete m_timer;
}

void TWorker::build()
{
    qDebug("starting TWorker....") ;
    m_timer=new QTimer(this);
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setInterval(m_sleeptime);
    connect(m_timer,&QTimer::timeout,this,&TWorker::work);
}

void TWorker::stop()
{
    emit finished();
    _stop=true;
}

void TWorker::restart()
{
    if(isStopped()){
        m_timer->start();
    }
}


bool TWorker::isStopped()
{
    return !m_timer->isActive();
}

void TWorker::setSleepTime(short dur)
{
    m_sleeptime=dur;
}

void TWorker::start(TWorker::Priority p)
{
    m_priority=p;
    switch (p) {
    case Low:
        m_sleeptime = sleepL;
        break;
    case Normal:
        m_sleeptime = sleepN;
        break;
    case High:
        m_sleeptime = sleepH;
        break;
    default:
        qDebug("Invalid priority");
        return;  // No action if invalid priority
    }

    // Set the timer interval and start it
    m_timer->setInterval(m_sleeptime);
    m_timer->start();
}

TWorker::Priority TWorker::priority()
{
    return m_priority;
}


void TWorker::work()
{
    if(_stop){
        m_timer->stop();
        thread()->quit();
    }
}


