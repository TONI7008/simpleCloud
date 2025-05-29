#include "tcloud.h"
#include "barupdater.h"
#include "uclient.h"

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

// In constructor, remove speedLimit initialization
UClient::UClient(QString username, QString filepath, BarUpdater *up, QObject *parent)
    : QObject(parent), m_filepath(filepath), m_username(username), m_updater(up) // removed: ,speedLimit(15)
{
    Q_UNUSED(parent);
    qDebug() << "uclient started ";
}

UClient::~UClient() {
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected();
        m_socket->deleteLater();
    }
    m_updater->deleteLater();
}

void UClient::run() {
    m_socket = new QTcpSocket(this);

    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1); // Low latency (optional for large files)
    //m_socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 4 * 1024 * 1024); // 4 MB
    m_socket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 4 * 1024 * 1024); // 4 MB

    m_socket->connectToHost(QHostAddress(TCLOUD::serverAdress), TCLOUD::receiveFilePort);


    connect(m_socket, &QTcpSocket::connected, this, [&] {
        filesize = getFileSize();

        m_info.filepath=m_sfilepath;
        m_info.size=filesize;
        m_info.lastEditDate=getLastModifiedDate();
        m_info.uploadDate=getCurrentDate();
        m_info.mimetype=getMimeType(m_filepath);

        m_socket->write("INIT\n");
        m_socket->write(m_username.toUtf8() + "\n");
        m_socket->write(m_sfilepath.toUtf8() + "\n");
        m_socket->write(QString("%1").arg(filesize).toUtf8() + "\n");
        m_socket->write(getCurrentDate().toString(Qt::ISODate).toUtf8() + "\n");
        m_socket->write(getLastModifiedDate().toString(Qt::ISODate).toUtf8() + "\n");
        m_socket->write(getMimeType(m_filepath).toUtf8() + "\n");

        m_socket->flush();
        m_socket->waitForBytesWritten();

        while (m_socket->bytesToWrite() > 0) {
            m_socket->waitForBytesWritten();
        }

        emit ready();
        TCLOUD::Wait(50);
    });

    connect(m_socket, &QTcpSocket::readyRead, this, &UClient::processData);
    connect(m_socket, &QTcpSocket::disconnected, this, &UClient::Disconnect);
    connect(m_socket, &QTcpSocket::errorOccurred, this, [=] {
        qDebug() << "error" << m_socket->error();
    });

    while (true) {
        if (stop) {
            break;
        }
        TCLOUD::Wait(50);
    }
    thread()->quit();
}


void UClient::sendingData() {
    QFile file(m_filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file for reading";
        return;
    }

    const qint64 BufferSize = 512 * 1024;

    QByteArray buffer;
    buffer.resize(BufferSize);

    while (!file.atEnd()) {
        if(paused){
            TCLOUD::Wait(500);
            continue;
        }
        qint64 lineLength = file.read(buffer.data(), BufferSize);
        qint64 bytesWritten = 0;


        while (bytesWritten < lineLength) {
            qint64 written = m_socket->write(buffer.constData() + bytesWritten, lineLength - bytesWritten);
            if (written == -1) {
                qDebug() << "Write failed";
                return;
            }
            bytesWritten += written;
            if (!m_socket->waitForBytesWritten(10000)) {
                qDebug() << "Timeout while writing";
                return;
            }
        }
        blocksize += bytesWritten;
        m_updater->setValue(blocksize);

    }

    file.close();
}

void UClient::processData() {
    m_data = m_socket->readAll();
    if (!started) {
        if (QString(m_data).split("\n")[0] == "READY") {
            sendingData();
            started = false;
        } else if (QString(m_data).split("\n")[0] == "SENDING DONE") {
            qDebug() << "sending done received";
            TCLOUD::Wait(100);
            stop = true;
        }
    }
}

QString UClient::getMimeType(const QString &filePath) {
    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

    if (mimeType.isValid()) {
        return mimeType.name();
    } else {
        return "application/octet-stream";
    }
}

QDateTime UClient::getLastModifiedDate() {
    if (m_filepath.isEmpty()) {
        return QDateTime();
    }

    QFileInfo fileInfo(m_filepath);
    return fileInfo.lastModified();
}

QDateTime UClient::getCurrentDate() {
    return QDateTime::currentDateTime();
}

qint64 UClient::getFileSize() {
    QFileInfo fileInfo(m_filepath);
    return fileInfo.size();
}


QString UClient::getFilename() {
    if (m_filepath.isEmpty()) {
        return QString();
    }

    QFileInfo fileInfo(m_filepath);
    return fileInfo.fileName();
}

void UClient::Disconnect() {
    m_socket->waitForDisconnected();
    stop = true;
}

void UClient::setCurrentPath(QString path){
    m_path = path;
    if(m_path[m_path.length()-1]=='/'){
        m_sfilepath = m_path + getFilename();

    }else{
        m_sfilepath = m_path +"/" +getFilename();
    }
}

void UClient::pause()
{
    paused=true;
}

void UClient::resume()
{
    paused=false;
}

void UClient::quit()
{
    stop=true;
}

