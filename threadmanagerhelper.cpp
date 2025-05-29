#include "threadmanagerhelper.h"
#include "threadmanager.h"

#include "uclient.h"
#include "dclient.h"
#include "mainthread.h"
#include "tcloud.h"
#include "loader.h"
#include "barupdater.h"

ThreadManagerHelper::ThreadManagerHelper(QObject *parent)
    : QObject{parent},m_timer(new QTimer(this)),m_value(0),c_value(0),fileSize(0)
{
    qDebug() << "helper started...";
    m_timer->setTimerType(Qt::PreciseTimer);
}

ThreadManagerHelper::~ThreadManagerHelper()
{
    //m_timer->stop();
    //delete m_timer;
}

void ThreadManagerHelper::Upload(ThreadManager *manager,QString filename, QString username, Loader* loader)
{
    BarUpdater* updater = new BarUpdater();
    UClient* client = new UClient(username, filename, updater);
    client->setCurrentPath(manager->m_path);

    connect(loader->progressBar(),&CircularProgressBar::stopEmitted,this,[client](bool paused){
        if(paused){
            client->pause();
        }else{
            client->resume();
        }
    });

    loader->setRange(0, 10000);
    loader->start();


    fileSize=client->getFileSize();

    m_timer->setSingleShot(true);

    QThread* thread1 = new QThread;
    QThread* thread2 = new QThread;

    updater->moveToThread(thread1);
    client->moveToThread(thread2);

    connect(thread1, &QThread::started, updater, &BarUpdater::run);
    connect(thread2, &QThread::started, client, &UClient::run);

    // Update loader progress and speed as the upload progresses
    connect(updater, &BarUpdater::increased, loader, [updater, loader,this]() {
        if (!m_timer->isActive()) {
            m_timer->start(500);
        }
        qint64 tmp = m_value;
        m_value = updater->getValue();
        c_value += m_value - tmp;

        int progressPercentage = static_cast<int>((m_value * 10000) / fileSize);
        loader->setValue(progressPercentage);

        if (m_value == fileSize) {
            updater->stop();
            m_timer->stop();
            m_timer->deleteLater();
        }
    }, Qt::QueuedConnection);

    connect(m_timer, &QTimer::timeout, this, [loader,this]() {
        loader->setSpeed(TCLOUD::formatSize(c_value*2) + "/s");
        loader->setProgress(TCLOUD::formatSize(m_value) + "/" + TCLOUD::formatSize(fileSize));
        c_value = 0;
    });

    connect(thread2, &QThread::finished, this, [=]() {
        emit manager->uploadDone(client->getInfo());
        manager->Ulist.removeOne(client);
        manager->uploadQueue.removeOne(queueItem(filename, username, loader));

        client->deleteLater();
        updater->deleteLater();
        thread1->quit();

        thread1->wait();
        thread2->wait();

        thread1->deleteLater();
        thread2->deleteLater();

        emit doneUploading();
    });

    thread1->start();
    thread2->start(QThread::HighPriority);

    manager->Ulist.append(client);

}

void ThreadManagerHelper::Download(ThreadManager *manager, QString filename, QString username, Loader *loader)
{
    BarUpdater* updater = new BarUpdater();
    DClient* client = new DClient(username, filename, updater,85);

    connect(loader->progressBar(),&CircularProgressBar::stopEmitted,this,[manager,client](bool paused){
        if(paused){
            manager->m_thread->pause(client->id(),"1");
        }else{
            manager->m_thread->pause(client->id(),"0");
        }
    });

    loader->start();
    loader->setRange(0, 10000);

    m_timer->setSingleShot(true);
    connect(client,&DClient::fileSizeReady,this,&ThreadManagerHelper::setFileSize);

    connect(m_timer, &QTimer::timeout, this, [loader,this]() {
        loader->setSpeed(TCLOUD::formatSize(c_value*2) + "/s");
        loader->setProgress(TCLOUD::formatSize(m_value) + "/" + TCLOUD::formatSize(fileSize));
    });

    QThread* thread1 = new QThread;
    QThread* thread2 = new QThread;

    updater->moveToThread(thread1);
    client->moveToThread(thread2);

    connect(thread1, &QThread::started, updater, &BarUpdater::run);
    connect(thread2, &QThread::started, client, &DClient::run);

    connect(updater, &BarUpdater::increased, loader, [updater, loader,this]() {
        if (!m_timer->isActive()) {
            m_timer->start(500);
            c_value = 0;
        }
        qint64 tmp = m_value;
        m_value = updater->getValue();
        c_value += m_value - tmp;

        int progressPercentage = static_cast<int>((m_value * 10000) / fileSize);
        loader->setValue(progressPercentage);
        if (m_value == fileSize) {
            updater->stop();
            m_timer->stop();
            m_timer->deleteLater();
        }
    }, Qt::QueuedConnection);

    connect(thread2, &QThread::finished, this, [=]() {
        manager->Dlist.removeOne(client);
        manager->downloadedFile.append(filePair(client->getFilename(), client->getFilePath()));

        client->deleteLater();
        updater->deleteLater();
        thread1->quit();
        thread2->quit();

        thread1->wait();
        thread2->wait();

        thread1->deleteLater();
        thread2->deleteLater();

        emit doneDownloading();
    });

    thread1->start();
    thread2->start(QThread::HighPriority);
    manager->Dlist.append(client);
}
