#ifndef UCLIENT_H
#define UCLIENT_H

#include <QDateTime>
#include <QTcpSocket>
#include "tfileinfo.h"

class BarUpdater;

class UClient : public QObject{
    Q_OBJECT
public:
    explicit UClient(QString username,QString filepath,BarUpdater *up,QObject* parent = nullptr);
    ~UClient();
    void processData();
    void Disconnect();
    void setCurrentPath(QString);

    qint64 getFileSize();
    qint64 getMaxSize(){
        return filesize;
    }
    QString getFilename();
    QDateTime getCurrentDate();
    QDateTime getLastModifiedDate();
    void sendingData();
    TFileInfo getInfo(){
        return m_info;
    }

    void pause();
    void resume();
    void quit();
signals:
    void ready();

public slots:
    void run();

private:
    QString m_filepath;
    QString tmpfilepath;
    QString m_path;
    QString m_username;
    QString m_sfilepath;
    TFileInfo m_info;
    QTcpSocket* m_socket=nullptr;
    bool stop=false;
    bool started=false;
    bool paused=false;


    qint64 filesize=0;
    qint64 blocksize=0;
    QByteArray m_data;
    BarUpdater* m_updater=nullptr;

    QString getMimeType(const QString &filePath);
};

#endif // UCLIENT_H
