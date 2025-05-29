#include "dclient.h"
#include "barupdater.h"
#include "tcloud.h"

#include <QDir>
#include <QUuid>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include "idmaker.h"

DClient::DClient(QString username, QString filename, BarUpdater *up,QObject *parent)
    : QObject(parent), fileName(filename), m_username(username), m_updater(up), speedLimit(15)
{
    filePath=fileName;
    fileName=fileName.mid(2).split("/").last();
    m_tempFilePath = makeTempFilePath();
    setId(IdMaker::makeId());
}

DClient::DClient(QString username, QString filename, BarUpdater *up,short limit,QObject *parent)
    : QObject(parent), fileName(filename), m_username(username), m_updater(up), speedLimit(limit) // Default speed limit to 5 MB/s
{
    filePath=fileName;
    fileName=fileName.mid(2).split("/").last();
    m_tempFilePath = makeTempFilePath();
    setId(IdMaker::makeId());
}

DClient::~DClient(){
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
    }
    m_updater->deleteLater();
}

QString DClient::generateUniqueFileName(const QString originalFileName) {
    QFileInfo fileInfo(originalFileName);
    QString extension = fileInfo.suffix();
    QString baseName = QUuid::createUuid().toString() + "_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    return sanitizeFileName(baseName + "." + extension);
}

QString DClient::sanitizeFileName(const QString _fileName) {
    QString sanitizedFileName = _fileName;
    sanitizedFileName.replace(QRegularExpression(R"([<>':"/\\|?*])"), "_");
    return sanitizedFileName;
}

QString DClient::getDownloadDirectory() {
    QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QDir downloadDir(downloadPath);
    if (!downloadDir.exists("TCloudFile")) {
        downloadDir.mkdir("TCloudFile");
    }
    return downloadDir.filePath("TCloudFile");
}

QString DClient::makeTempFilePath() {
    QString downloadDir = getDownloadDirectory();
    QString uniqueFileName = fileName;
    return QDir(downloadDir).filePath(uniqueFileName);
}

void DClient::run() {
    m_socket = new QTcpSocket(this);
    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1); // Low latency (optional for large files)
    m_socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 4 * 1024 * 1024); // 4 MB

    m_socket->connectToHost(QHostAddress(TCLOUD::serverAdress), TCLOUD::sendFilePort);

    connect(m_socket, &QTcpSocket::readyRead, this, &DClient::processData);
    connect(m_socket, &QTcpSocket::disconnected, this, &DClient::Disconnect);
    connect(m_socket, &QTcpSocket::connected, this, &DClient::onConnected);

    while (true) {
        if (stop) {
            break;
        }
        TCLOUD::Wait(75);
    }
    thread()->quit();
}

void DClient::onConnected() {
    setSpeedLimit(speedLimit);
    TCLOUD::Wait(150);
    m_socket->write("INIT\n");
    m_socket->write(m_username.toUtf8() + "\n");
    m_socket->write(filePath.toUtf8());

    m_socket->flush();
    m_socket->waitForBytesWritten();
    while (m_socket->bytesToWrite() > 0) {
        m_socket->waitForBytesWritten();
    }

    emit ready();
    TCLOUD::Wait(75);
}

void DClient::setSpeedLimit(short limit) {
    _setLimit(limit);
}
void DClient::_setLimit(short limit){
    m_socket->write("SETLIMIT\n");
    m_socket->write(QString("%1").arg(limit).toUtf8()+"\n");
    m_socket->write(m_id+"\n");
    m_socket->flush();
    m_socket->waitForBytesWritten();
}

void DClient::setId(QByteArray id)
{
    if(!idSet){
        m_id=id;
        idSet=true;
    }
}
void DClient::receivingData() {
    QFile file(m_tempFilePath);

    if (!file.open(QIODevice::Append)) {
        qDebug() << "Error: Failed to open file for appending" << m_tempFilePath;
        return;
    }

    while (m_socket->bytesAvailable() > 0) {
        QByteArray buffer = m_socket->readAll();
        qint64 dataSize = buffer.size();
        blockSize += dataSize;

        qint64 bytesWritten = file.write(buffer);
        if (bytesWritten < 0) {
            qDebug() << "Error: Failed to write data to file" << m_tempFilePath;
        } else if (bytesWritten < dataSize) {
            qDebug() << "Warning: Not all data was written to file" << m_tempFilePath;
        }

        m_updater->setValue(blockSize);

        if (blockSize >= fileSize) {
            finalizeDownload();
            break;
        }
    }
    file.close();
}

void DClient::processData() {
    if (!started) {
        m_data = m_socket->readAll();
        QStringList parts = QString(m_data).split("\n");
        if (parts[0] == "INIT") {
            parts.removeFirst();
            fileSize = parts[0].toLongLong();
            emit fileSizeReady(fileSize);
            TCLOUD::Wait(150);
            uploadTime = QDateTime::fromString(parts[1], Qt::ISODate);
            lastModifiedDate = QDateTime::fromString(parts[2], Qt::ISODate);
            mime = parts[3];
            started = true;

            m_socket->write("READY\n");
            m_socket->flush();
            m_socket->waitForBytesWritten();
        }
    } else {
        receivingData();
    }
}

void DClient::finalizeDownload() {
    //qDebug() << "blockSize=" << blockSize;
    started = false;
    blockSize = 0;

    m_socket->write("SENDING DONE\n");
    m_socket->flush();
    m_socket->waitForBytesWritten();
    TCLOUD::Wait(150);

    qDebug() << "Done";

    TCLOUD::Wait(200);

    stop = true;
}

void DClient::Disconnect() {
    stop = true;
    m_socket->waitForDisconnected();
}
