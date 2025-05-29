#include "threadmanager.h"
#include "threadmanagerhelper.h"
#include "settingmanager.h"
#include "loader.h"
#include "tfolder.h"

ThreadManager::ThreadManager(mainThread* th,QObject *parent)
    : QThread{parent},
    m_manager(nullptr),
    m_thread(th)
{
    limit_download=100;
    limit_upload=100;
}

ThreadManager::~ThreadManager()
{
    if(m_manager){
        m_manager=nullptr;
        delete m_manager;
    }
}

void ThreadManager::setManager(SettingsManager* manager){
    if (m_manager) {
        m_manager = manager;
        limit_upload = m_manager->uploadLimit();
        limit_download = m_manager->downloadLimit();
        connect(m_manager,&SettingsManager::downloadLimitChanged,this,&ThreadManager::setDownloadLimit);
        connect(m_manager,&SettingsManager::uploadLimitChanged,this,&ThreadManager::setUploadLimit);
    }
}

void ThreadManager::upload(QString filename, QString username,Loader* loader)
{
    setPath(TFolder::FolderHistory[TFolder::currentFolderIndex]->path());
    if (uploadQueue.size() < limit_upload) {
        startUpload(filename, username,loader);
    } else {
        uploadQueue.append(queueItem(filename, username,loader));
    }
}

void ThreadManager::startUpload(QString filename, QString username, Loader* loader) {
    ThreadManagerHelper* helper=new ThreadManagerHelper;
    connect(helper,&ThreadManagerHelper::doneUploading,this,[this,helper]{
        qDebug() << "upload queue size: " << downloadQueue.size();

        if (!uploadQueue.isEmpty()) {
            auto nextUpload = uploadQueue.takeLast();
            startUpload(nextUpload.first(), nextUpload.second(), nextUpload.third());
        }
        helper->deleteLater();
    });
    helper->Upload(this,filename,username,loader);
}

void ThreadManager::download(QString filename, QString username,Loader* loader)
{
    setPath(TFolder::FolderHistory[TFolder::currentFolderIndex]->path());
    if (downloadQueue.size() < limit_download) {
        startDownload(filename, username,loader);
        qDebug("downloading....");
    } else {
        downloadQueue.append(queueItem(filename, username,loader));
        qDebug("will downloaded shortly ....");
    }
}

void ThreadManager::downloadII(QString fname, QString username, Loader *loader)
{
    loader->setLoop(true);
    QTimer *timer = new QTimer(this); // Set the parent to manage memory
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this, timer, fname, username, loader] {
        qDebug() << "size dlist =" << Dlist.size();

        if (Dlist.size() < limit_download) {
            loader->setLoop(false);
            startDownloadII(fname, username, loader);
            timer->deleteLater();
        } else {
            timer->start(200);
        }
    });

    timer->start(200);
}

void ThreadManager::startDownload(QString filename, QString username,Loader* loader)
{
    ThreadManagerHelper* helper=new ThreadManagerHelper;
    connect(helper,&ThreadManagerHelper::doneDownloading,this,[this,helper]{
        qDebug() << "download queue size" << downloadQueue.size();
        if (!downloadQueue.isEmpty()) {
            auto nextDownload = downloadQueue.takeFirst();
            startDownload(nextDownload.first(), nextDownload.second(), nextDownload.third());
        }
        helper->deleteLater();
    });
    helper->Download(this,filename,username,loader);
}

void ThreadManager::startDownloadII(QString filename, QString username, Loader *loader) {
    ThreadManagerHelper* helper=new ThreadManagerHelper;
    connect(helper,&ThreadManagerHelper::doneDownloading,this,[this,helper]{
        if (!downloadQueue.isEmpty()) {
            auto nextDownload = downloadQueue.takeFirst();
            startDownload(nextDownload.first(), nextDownload.second(), nextDownload.third());
        }
        helper->deleteLater();
    });
    helper->Download(this,filename,username,loader);
}

void ThreadManager::Start(){

    if (stop) {
        thread()->quit();
    }
    exec();
}

void ThreadManager::run()
{

}

void ThreadManager::setPath(QString npath)
{
    m_path = npath;
}

void ThreadManager::openFile(QString filename)
{
    for (filePair &pair : downloadedFile) {
        if (pair.filename == filename) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(pair.filepath));
            break;
        }
    }
}

void ThreadManager::setUploadLimit(short limit) { limit_upload = limit; }

void ThreadManager::setDownloadLimit(short limit) { limit_download = limit; }
