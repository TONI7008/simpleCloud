#include "networkagent.h"
#include "tcloud.hpp"
#include "tcompresser.h"
#include "tsecuritymanager.h"
#include "tfileinfo.hpp"
#include "communicationdispatcher.h"

void NetworkAgent::sendEncryptedData(const QByteArray& data) {
    qint64 size = data.size();
    QByteArray sizeArray;
    QDataStream out(&sizeArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << size;

    QByteArray packet = sizeArray + data;
    m_dataQueue.enqueue(packet);
    trySendNext();
}

void NetworkAgent::trySendNext() {
    if (m_waitingForResponse || m_dataQueue.isEmpty())
        return;

    QByteArray packet = m_dataQueue.head();
    m_socket->write(packet);
    m_socket->flush();
    m_socket->waitForBytesWritten();

    m_waitingForResponse = true;
}

void NetworkAgent::acknowledgeResponse() {
    if (!m_dataQueue.isEmpty())
        m_dataQueue.dequeue();

    m_waitingForResponse = false;
    trySendNext();
}

NetworkAgent::NetworkAgent(TSecurityManager* m,QObject *parent)
    : TWorker{parent},m_Smanager(m),separator("\x1F\x1E\x1D\x1C"),expectedSize(0), sizeReceived(false)
{
    qDebug() << "Starting NetworkAgent......";
}

NetworkAgent::~NetworkAgent(){
    m_socket->disconnectFromHost();
    m_socket->deleteLater();
    delete m_Smanager;
    if(!isStopped()) stop();
}

void NetworkAgent::work(){
    TWorker::work();

    if (first) {
        m_socket = new QSslSocket(this);
        m_socket->setSslConfiguration(m_Smanager->config());

        m_socket->connectToHostEncrypted(TCLOUD::serverAdress, TCLOUD::port);

        connect(m_socket, &QSslSocket::encrypted, this, [&] {
            emit connected(true);
        });

        connect(m_socket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&](const QList<QSslError>& errors) {
            for (const QSslError& error : errors) {
                qDebug() << "SSL Error:" << error.errorString();
            }
            emit connected(false);
        });
        connect(m_socket,&QSslSocket::readyRead,this,&NetworkAgent::processDataHelper);

        first = false;
    }

    if(_init){
        if(signalDataString.isEmpty()){
            init();
        }
        signalDataString.clear();
        _init=false;
    }
    if(_createDir){
        if(signalDataString.size()==1){
            CreateDir(signalDataString.at(0));
        }
        signalDataString.clear();
        _createDir=false;
    }
    if(_synchronizeSetting){
        if(signalDataByte.size()==2){
            synchronizeSetting(signalDataByte.at(0),signalDataByte.at(1));
        }
        signalDataByte.clear();
        _synchronizeSetting=false;
    }
    if(_login){
        if(signalDataString.size()==2){
            Login(signalDataString.at(0),signalDataString.at(1));
        }
        signalDataString.clear();
        _login=false;
    }
    if(_signup){
        if(signalDataString.size()==3){
            SignUp(signalDataString.at(0),signalDataString.at(1),signalDataString.at(2));
        }
        signalDataString.clear();
        _signup=false;
    }
    if(_signupVerif){
        if(signalDataString.size()==4){
            SignUpVerification(signalDataString.at(0),signalDataString.at(1),signalDataString.at(2),signalDataString.at(3));
        }
        signalDataString.clear();
        _signupVerif=false;
    }
    if(_renamefile){
        if(signalDataString.size()==2){
            RenameFile(signalDataString.at(0),signalDataString.at(1));
        }
        signalDataString.clear();
        _renamefile=false;
    }
    if(_renamefolder){
        if(signalDataString.size()==2){
            RenameFolder(signalDataString.at(0),signalDataString.at(1));
        }
        signalDataString.clear();
        _renamefolder=false;
    }
    if(_softDeleteFile){
        if(signalDataString.size()==1){
            SoftDeleteFile(signalDataString.at(0));
        }
        signalDataString.clear();
        _softDeleteFile=false;
    }
    if(_softDeleteFolder){
        if(signalDataString.size()==1){
            SoftDeleteFolder(signalDataString.at(0));
        }
        signalDataString.clear();
        _softDeleteFolder=false;
    }
    if(_restoreFile){
        if(signalDataString.size()==1){
            RestoreFile(signalDataString.at(0));
        }
        signalDataString.clear();
        _restoreFile=false;
    }
    if(_restoreFolder){
        if(signalDataString.size()==1){
            RestoreFolder(signalDataString.at(0));
        }
        signalDataString.clear();
        _restoreFolder=false;
    }
    if(_deletefile){
        if(signalDataString.size()==1){
            DeleteFile(signalDataString.at(0));
        }
        signalDataString.clear();
        _deletefile=false;
    }
    if(_deletefolder){
        if(signalDataString.size()==1){
            DeleteFolder(signalDataString.at(0));
        }
        signalDataString.clear();
        _deletefolder=false;
    }
    if(_searchuser){
        if(signalDataString.size()==1){
            SearchUser(signalDataString.at(0));
        }
        signalDataString.clear();
        _searchuser=false;
    }
    if(_paused){
        if(signalDataByte.size()==2){
            Pause(signalDataByte.at(0),signalDataByte.at(1));
        }
        signalDataByte.clear();
        _paused=false;
    }
    if(_cutFile){
        if(signalDataString.size()==2){
            CutFile(signalDataString.at(0),signalDataString.at(1));
        }
        signalDataString.clear();
        _cutFile=false;
    }
    if(_copyFile){
        if(signalDataString.size()==2){
            CopyFile(signalDataString.at(0),signalDataString.at(1));
        }
        signalDataString.clear();
        _copyFile=false;
    }
    if(_cutFolder){
        if(signalDataString.size()==2){
            CutFolder(signalDataString.at(0),signalDataString.at(1));
        }
        signalDataString.clear();
        _cutFolder=false;
    }
    if(_copyFolder){
        if(signalDataString.size()==2){
            CopyFolder(signalDataString.at(0),signalDataString.at(1));
        }
        signalDataString.clear();
        _copyFolder=false;
    }
    if(_deleteAccount){
        DeleteAccount();
        signalDataString.clear();
        _deleteAccount=false;
    }

}

void NetworkAgent::processData()
{
    QByteArray d = m_data;
    QString message = QString(d);

    // Use the static dispatchMessage to get the message type
    CommunicationDispatcher::MessageType type =
        CommunicationDispatcher::dispatchMessage(message);

    switch (type) {
    case CommunicationDispatcher::INIT_OK:
        processInit();
        break;

    case CommunicationDispatcher::LOGIN_OK:
        emit loginSuccess();
        m_availableSpace = QString(message.split("\n")[1]).toLongLong();
        break;

    case CommunicationDispatcher::LOGIN_FAILED:
        emit loginFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::REGISTER_FAILED:
        emit registerFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::REGISTER_CONTINUE:
        emit registerContinue();
        break;

    case CommunicationDispatcher::REGISTER_OK:
        emit registerSuccess();
        break;

    case CommunicationDispatcher::DELETE_FAILED:
        emit deleteFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::DELETE_OK:
        emit deleteSuccess();
        break;

    case CommunicationDispatcher::RENAME_FAILED:
        emit renameFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::RENAME_OK:
        emit renameSuccess();
        break;

    case CommunicationDispatcher::SOFTDELETE_FAILED:
        emit deleteFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::SOFTDELETE_OK:
        emit deleteSuccess();
        break;

    case CommunicationDispatcher::CREATEDIR_FAILED:

        emit createDirFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::CREATEDIR_OK:
        emit createDirSuccess();
        break;

    case CommunicationDispatcher::SEARCH_OK:
        processSearchData();
        break;

    case CommunicationDispatcher::COPY_OK:
        emit copySuccess();
        break;

    case CommunicationDispatcher::CUT_OK:
        emit cutSuccess();
        break;

    case CommunicationDispatcher::COPY_FAILED:
        emit copyFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::CUT_FAILED:
        emit cutFailed(message.split("\n")[1]);
        break;

    case CommunicationDispatcher::RESTORE_OK:
        emit restoreSuccess();
        break;

    case CommunicationDispatcher::RESTORE_FAILED:
        emit restoreFailed(message.split("\n")[1]);
        break;
    case CommunicationDispatcher::SYNCHRONIZESETTINGS_FAILED:
        emit synchronizeFailed(message.split("\n")[1]);
        break;
    case CommunicationDispatcher::SYNCHRONIZESETTINGS_OK:
        emit synchronizeSuccess();
        break;
    case CommunicationDispatcher::DELETEACCOUNT_OK:
        emit deleteAccountSuccess();
        break;
    case CommunicationDispatcher::DELETEACCOUNT_FAILED:
        emit deleteAccountFailed(message.split("\n")[1]);
        break;

    default:
        // Handle unknown message types
        qDebug() << "Unknown message type received:" << message;
        break;
    }

    acknowledgeResponse();
}

QStringList NetworkAgent::getErrors()
{
    return m_errorList;
}



bool NetworkAgent::waitingForResponse() const
{
    return m_waitingForResponse;
}


QByteArray NetworkAgent::getUserSetting()
{
    return m_userSetting;
}

QByteArray NetworkAgent::getUserProfile()
{
    return m_userProfile;
}

void NetworkAgent::processSearchData() {

    QList<userData> userList;
    QByteArray recordSeparator = "||<RECORD_SEPARATOR>||";
    QByteArray fieldSeparator = "||<FIELD_SEPARATOR>||";

    // Remove the "SEARCH OK\n" part before processing the actual data
    if (m_data.startsWith("SEARCH OK\n")) {
        m_data.remove(0, QByteArray("SEARCH OK\n").size());
    }
    TCompresser compresser;
    m_data=compresser.decompressData(m_data);

    // Manual splitting using the full recordSeparator
    int startIndex = 0;
    while (true) {
        // Find the index of the next record separator
        int recordEnd = m_data.indexOf(recordSeparator, startIndex);

        // If no more separators are found, break the loop
        if (recordEnd == -1) {
            recordEnd = m_data.size();
        }

        // Extract the record data between startIndex and recordEnd
        QByteArray record = m_data.mid(startIndex, recordEnd - startIndex);

        // Split the record into fields using the full fieldSeparator
        int fieldIndex = record.indexOf(fieldSeparator);
        if (fieldIndex != -1) {
            // Extract the username and picture fields
            QByteArray usernameData = record.left(fieldIndex);  // Data before the field separator is the username
            QByteArray pictureData = record.mid(fieldIndex + fieldSeparator.size());  // Data after is the picture

            // Convert the username to a QString (UTF-8 encoded)
            QString username = QString::fromUtf8(usernameData);

            // Add the userData to the list
            userList.append(userData(username, pictureData));
        }

        // Move to the next record
        if (recordEnd == m_data.size()) {
            break;
        }
        startIndex = recordEnd + recordSeparator.size();  // Skip over the separator
    }

    qDebug("search done emitted");

    emit searchDone(userList);
}

qint64 NetworkAgent::getAvailableSpace() const
{
    return m_availableSpace;
}

QString NetworkAgent::getUsername(){return username;}

void NetworkAgent::processDataHelper()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_6_5);

    if (!sizeReceived) {
        if (m_socket->bytesAvailable() < static_cast<qint64>(sizeof(qint64))) {
            return;
        }
        in >> expectedSize;
        sizeReceived = true;
    }

    if (m_socket->bytesAvailable() < expectedSize) {
        return;
    }

    QByteArray encryptedData = m_socket->read(expectedSize);


    if (!m_data.isEmpty()) m_data.clear();
    m_data.append(encryptedData);

    sizeReceived = false;
    expectedSize = 0;

    processData();
}

void NetworkAgent::userInit(){
    _init=true;
}

void NetworkAgent::init(){
    QByteArray data;
    data.append("INIT\n");

    sendEncryptedData(data);
}

void NetworkAgent::login(QString usern, QString passw){
    signalDataString << usern << passw;
    _login=true;
}

void NetworkAgent::Login(QString usern, QString passw){
    QByteArray logindata;
    logindata.append("LOGIN\n");
    logindata.append(usern.toUtf8() + "," + passw.toUtf8());
    logindata.append("\n");

    sendEncryptedData(logindata);
}

void NetworkAgent::SearchUser(QString name)
{
    QByteArray searchdata;
    searchdata.append("SEARCHUSER\n");
    searchdata.append(name.toUtf8());
    searchdata.append("\n");

    sendEncryptedData(searchdata);
}

void NetworkAgent::signUp(QString usern, QString email, QString passw){
    signalDataString << usern << email << passw;
    _signup=true;
}

void NetworkAgent::signUpConfirm(QString usern, QString email, QString passw, QString verifNumber)
{
    signalDataString << usern << email << passw << verifNumber;
    _signupVerif=true;
}

void NetworkAgent::SignUp(QString usern, QString email, QString passw){
    QByteArray registerdata;
    registerdata.append("REGISTER\n");
    registerdata.append(usern.toUtf8() + "," + passw.toUtf8() + "," + email.toUtf8()+"\n");
    registerdata.append("FirstTry\n");

    // Prepend the size of the data
    sendEncryptedData(registerdata);
}

void NetworkAgent::SignUpVerification(QString usern, QString email, QString passw, QString vNum)
{
    QByteArray registerdata;
    registerdata.append("REGISTER\n");
    registerdata.append(usern.toUtf8() + "," + passw.toUtf8() + "," + email.toUtf8()+"\n");
    registerdata.append(vNum.toUtf8()+"\n");

    // Prepend the size of the data
    sendEncryptedData(registerdata);

}

void NetworkAgent::renameFile(QString OldName, QString NewName){
    signalDataString << OldName << NewName;
    _renamefile=true;

}

void NetworkAgent::renameFolder(QString OldName, QString NewName)
{
    signalDataString << OldName << NewName;
    _renamefolder=true;
}

void NetworkAgent::pause(QByteArray ptr,QByteArray bl)
{
    signalDataByte << ptr << bl;
    _paused=true;
}

void NetworkAgent::RenameFile(QString OldName, QString NewName){
    QByteArray data;
    data.append("RENAMEFILE\n");
    data.append(OldName.toUtf8() + "\n");
    data.append(NewName.toUtf8() + "\n");

    sendEncryptedData(data);

}

void NetworkAgent::RenameFolder(QString OldName, QString NewName)
{
    QByteArray data;
    data.append("RENAMEFOLDER\n");
    data.append(OldName.toUtf8() + "\n");
    data.append(NewName.toUtf8() + "\n");

    sendEncryptedData(data);
}

void NetworkAgent::CutFile(QString from, QString to)
{
    QByteArray data;
    data.append("CUTFILE\n");
    data.append(from.toUtf8() + "\n");
    data.append(to.toUtf8() + "\n");

    sendEncryptedData(data);
}

void NetworkAgent::CopyFile(QString from, QString to)
{
    QByteArray data;
    data.append("COPYFILE\n");
    data.append(from.toUtf8() + "\n");
    data.append(to.toUtf8() + "\n");

    sendEncryptedData(data);
}
void NetworkAgent::CutFolder(QString from, QString to)
{
    QByteArray data;
    data.append("CUTFOLDER\n");
    data.append(from.toUtf8() + "\n");
    data.append(to.toUtf8() + "\n");

    sendEncryptedData(data);
}

void NetworkAgent::CopyFolder(QString from, QString to)
{
    QByteArray data;
    data.append("COPYFOLDER\n");
    data.append(from.toUtf8() + "\n");
    data.append(to.toUtf8() + "\n");

    sendEncryptedData(data);
}

void NetworkAgent::copyFile(QString from, QString to)
{
    signalDataString << from << to;
    _copyFile=true;

}

void NetworkAgent::cutFile(QString from, QString to)
{
    signalDataString << from << to;
    _cutFile=true;

}
void NetworkAgent::copyFolder(QString from, QString to)
{
    signalDataString << from << to;
    _copyFolder=true;

}

void NetworkAgent::cutFolder(QString from, QString to)
{
    signalDataString << from << to;
    _cutFolder=true;
}

void NetworkAgent::Pause(QByteArray ptr,QByteArray booleen)
{
    QByteArray data;
    data.append("PAUSEUPLOAD\n");
    data.append(ptr + "\n");
    data.append(booleen + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);
}

void NetworkAgent::DeleteAccount()
{
    QByteArray data;
    data.append("DELETEACCOUNT\n");
    // Prepend the size of the data
    sendEncryptedData(data);

}

void NetworkAgent::deleteFile(QString fileName){
    signalDataString << fileName;
    _deletefile=true;
}

void NetworkAgent::deleteFolder(QString fileName)
{
    signalDataString << fileName;
    _deletefolder=true;
}

void NetworkAgent::restoreFile(QString fileName)
{
    signalDataString << fileName;
    _restoreFile=true;
}

void NetworkAgent::restoreFolder(QString fileName)
{
    signalDataString << fileName;
    _restoreFolder=true;

}

void NetworkAgent::DeleteFile(QString fileName){
    QByteArray data;
    data.append("DELETEFILE\n");
    data.append(fileName.toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);
}
void NetworkAgent::softDeleteFile(QString fileName){
    signalDataString << fileName;
    _softDeleteFile=true;
}

void NetworkAgent::deleteAccount()
{
    _deleteAccount=true;
}

void NetworkAgent::SoftDeleteFile(QString fileName){
    QByteArray data;
    data.append("SOFTDELETEFILE\n");
    data.append(fileName.toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);
}

void NetworkAgent::softDeleteFolder(QString fileName){
    signalDataString << fileName;
    _softDeleteFolder=true;
}

void NetworkAgent::SoftDeleteFolder(QString fileName){
    QByteArray data;
    data.append("SOFTDELETEFOLDER\n");
    data.append(fileName.toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);
}

void NetworkAgent::RestoreFile(QString fileName)
{
    QByteArray data;
    data.append("RESTORE\n");
    data.append(fileName.toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);

}

void NetworkAgent::RestoreFolder(QString fileName)
{
    QByteArray data;
    data.append("RESTORE\n");
    data.append(fileName.toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);
}

void NetworkAgent::DeleteFolder(QString fileName)
{
    QByteArray data;
    data.append("DELETEFOLDER\n");
    data.append(fileName.toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);

}

void NetworkAgent::moveFile(QString fileName)
{
    QByteArray data;
    data.append("DELETEFILE\n");
    data.append(fileName.toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);
}

void NetworkAgent::createDir(QString dirname){
    signalDataString.append(dirname);
    _createDir=true;
}

void NetworkAgent::searchUser(QString name)
{
    signalDataString.append(name);
    _searchuser=true;
}

QList<TFileInfo> NetworkAgent::getList(){return m_list;}

void NetworkAgent::CreateDir(QString dirname){
    QByteArray data;
    data.append("CREATEDIR\n");
    data.append(dirname.toUtf8() + "\n");
    data.append(QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8() + "\n");

    // Prepend the size of the data
    sendEncryptedData(data);

}

void NetworkAgent::synchronize(QByteArray setting, QByteArray pic) {
    signalDataByte.append(setting);
    signalDataByte.append(pic);
    _synchronizeSetting=true;
}

void NetworkAgent::synchronizeSetting(QByteArray setting, QByteArray pic) {
    QByteArray separator = "\x1F\x1E\x1D\x1C";  // Unique separator unlikely to appear in data
    QByteArray tosend;
    tosend.append("SYNCHRONIZESETTINGS\n");
    tosend.append(separator);
    tosend.append(setting);
    tosend.append(separator);
    tosend.append(pic);
    tosend.append(separator);

    sendEncryptedData(tosend);
}


void NetworkAgent::processInit() {
    QByteArray separator = "\x1F\x1E\x1D\x1C";

    QList<QByteArray> tempDataList;
    int startPos = 0;
    int separatorPos;

    while ((separatorPos = m_data.indexOf(separator, startPos)) != -1) {
        tempDataList.append(m_data.mid(startPos, separatorPos - startPos));
        startPos = separatorPos + separator.size();
    }
    // Add the remaining data after the last separator
    tempDataList.append(m_data.mid(startPos));

    if (tempDataList.size() < 3) {
        qDebug() << "Error: Expected at least 3 sections in data";
        return;
    }

    QString fileDataString = QString::fromUtf8(tempDataList[0]);
    QStringList fileDataList = fileDataString.split("\n");

    m_userSetting = tempDataList[1];  // Binary data, no conversion

    // The third section (tempDataList[2]) contains m_userProfile
    m_userProfile = tempDataList[2];  // Binary data, no conversion

    // Remove the first and last elements from the fileDataList (which are likely headers or footers)
    fileDataList.removeFirst();
    fileDataList.removeLast();

    if (!m_list.isEmpty()) {
        m_list.clear();
    }

    for (const QString &elt : std::as_const(fileDataList)) {
        QStringList fields = elt.split("[|]");
        if (fields.size() == 6) {
            TFileInfo info;
            info.filepath = fields[0];
            info.size = fields[1].toDouble();
            info.mimetype = fields[4];
            info.deleted=fields[5]=="1";

            // Define the date formats to try
            QStringList formats = {"ddd MMM dd HH:mm:ss yyyy", "ddd MMM d HH:mm:ss yyyy"};
            bool dateConverted = false;

            // Try to convert the upload date
            QString uploadDateString = fields[2];
            foreach (const QString &format, formats) {
                info.uploadDate = QDateTime::fromString(uploadDateString, format);
                if (info.uploadDate.isValid()) {
                    dateConverted = true;
                    break;
                }
            }

            if (!dateConverted) {
                qDebug() << "Error: Invalid uploadDate for string:" << uploadDateString;
            }

            QString lastEditDateString = fields[3];
            dateConverted = false;
            foreach (const QString &format, formats) {
                info.lastEditDate = QDateTime::fromString(lastEditDateString, format);
                if (info.lastEditDate.isValid()) {
                    dateConverted = true;
                    break;
                }
            }

            if (!dateConverted) {
                qDebug() << "Error: Invalid lastEditDate for string:" << lastEditDateString;
            }

            m_list.append(info);
        } else {
            qDebug() << "Error: Unexpected fields size: " << fields.size();
        }
    }

    TCLOUD::Wait(150);

    emit initSuccess();
}


void NetworkAgent::StartLoading(const QString& text){
    emit startLoading(text);
}

void NetworkAgent::StopLoading(){
    emit stopLoading();
}

void NetworkAgent::_refresh(){
    emit startLoading("refreshing ui...");
    TCLOUD::Wait(250);
    emit refresh();
}


