#ifndef DCLIENT_H
#define DCLIENT_H

#include <QDateTime>
#include <QTcpSocket>
#include <QObject>

class BarUpdater;

class DClient : public QObject {
    Q_OBJECT
public:
    explicit DClient(QString username, QString filename, BarUpdater* up, QObject* parent = nullptr);
    explicit DClient(QString username, QString filename, BarUpdater *up,short limit, QObject* parent = nullptr);
    ~DClient();
    void processData();
    void Disconnect();

    qint64 getFileSize() { return fileSize; }
    QString getFilename() { return filePath; }
    QString getFilePath() { return m_tempFilePath; }
    QString generateUniqueFileName(const QString originalFileName);

    QString sanitizeFileName(const QString _fileName);

    QString getDownloadDirectory();

    QString makeTempFilePath();

    void receivingData();
    qintptr socketDescriptor(){
        return m_socket->socketDescriptor();
    }

    void onConnected();
    void setSpeedLimit(short);
    QByteArray id(){return m_id;}
public slots:
    void run();

signals:
    void ready();
    void fileSizeReady(qint64);

private:
    QString m_tempFilePath="";
    QString fileName;
    QString filePath;
    QString m_username;
    QTcpSocket* m_socket;
    bool stop = false;
    bool started = false;
    QByteArray m_data;
    qint64 fileSize = 0;
    qint64 blockSize = 0;
    BarUpdater* m_updater;
    QDateTime uploadTime;
    QDateTime lastModifiedDate;
    QString mime;
    short speedLimit;
    void finalizeDownload();
    void _setLimit(short limit);
    QByteArray m_id;
    bool idSet=false;
    void setId(QByteArray);
};

#endif // DCLIENT_H
