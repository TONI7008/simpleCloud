#ifndef THREADMANAGERHELPER_H
#define THREADMANAGERHELPER_H

#include <QThread>
#include <QTimer>

class Loader;
class ThreadManager;

class ThreadManagerHelper : public QObject
{
    Q_OBJECT
public:
    explicit ThreadManagerHelper(QObject *parent = nullptr);
    ~ThreadManagerHelper();
    void Upload(ThreadManager*,QString filename, QString username, Loader* loader);
    void Download(ThreadManager*,QString filename, QString username, Loader* loader);
    void setFileSize(qint64 fs);
signals:
    void doneUploading();
    void doneDownloading();
private:
    QTimer* m_timer=nullptr;
    qint64 m_value,c_value,fileSize;
};

#endif // THREADMANAGERHELPER_H
