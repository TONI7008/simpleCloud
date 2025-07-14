#ifndef NetworkAgent_H
#define NetworkAgent_H


#include "tworker.h"
#include <QTcpSocket>
#include <QSslSocket>
#include <QQueue>

class TSecurityManager;
class TFileInfo;

struct userData
{
    userData(QString name,QByteArray pic) : m_name(name),m_pic(pic) {}
    QString m_name;
    QByteArray m_pic;
};

class NetworkAgent : public TWorker{
    Q_OBJECT
public:
    explicit NetworkAgent(TSecurityManager* m,QObject *parent = nullptr);
    ~NetworkAgent();

    qint64 getAvailableSpace() const;

    QString getUsername();
    void login(QString usern,QString passw);
    void signUp(QString usern,QString email,QString passw);
    void signUpConfirm(QString usern,QString email,QString passw,QString verifNumber);
    void userInit();
    void renameFile(QString OldName,QString NewName);
    void renameFolder(QString OldName,QString NewName);
    void pause(QByteArray ptr,QByteArray booleen);
    void deleteFile(QString fileName);
    void deleteFolder(QString fileName);
    void restoreFile(QString fileName);
    void restoreFolder(QString fileName);
    void softDeleteFile(QString fileName);
    void deleteAccount();
    void createDir(QString dirname);
    void searchUser(QString);

    void copyFile(QString from, QString to);
    void cutFile(QString from, QString to);
    void copyFolder(QString from, QString to);
    void cutFolder(QString from, QString to);

    QList<TFileInfo> getList();
    void processInit();
    void StartLoading(const QString&);

    void StopLoading();
    void setUsername(QString name){username=name;}

    void _refresh();
    QByteArray getUserSetting();
    QByteArray getUserProfile();

    void synchronize(QByteArray,QByteArray);

    void init();
    void softDeleteFolder(QString fileName);


    bool waitingForResponse() const;


    void setBlockSignalCount(short newBlockSignalCount);
    QStringList getErrors();

protected:
    void work() override;

signals:
    void serverAlert(const QString& alertMessage);
    void updateNotification(const QStringList& updateData);

    void refresh();
    void loginSuccess();
    void loginFailed(QString);

    void copySuccess();
    void copyFailed(QString);

    void cutSuccess();
    void cutFailed(QString);

    void registerSuccess();
    void registerFailed(QString);
    void restoreSuccess();
    void restoreFailed(QString);
    void registerContinue();

    void initSuccess();
    void renameSuccess();
    void renameFailed(QString);
    void createDirSuccess();
    void createDirFailed(QString);
    void deleteSuccess();
    void deleteFailed(QString);
    void startLoading(const QString&);
    void stopLoading();
    void startClient();
    void uploadDone();
    void searchDone(QList<userData>);
    void connected(bool);

    void completed(bool);
    void synchronizeSuccess();
    void synchronizeFailed(QString);
    void deleteAccountSuccess();
    void deleteAccountFailed(QString);


private:
    QQueue<QByteArray> m_dataQueue;
    bool m_waitingForResponse = false;

    void trySendNext();
    void acknowledgeResponse();

    QSslSocket* m_socket;
    TSecurityManager* m_Smanager;
    QByteArray m_data;

    QByteArray m_userSetting;
    QByteArray m_userProfile;

    qint64 m_availableSpace;
    QString username;

    QList<TFileInfo> m_list;
    QStringList m_errorList;

    QStringList signalDataString;
    QByteArrayList signalDataByte;

    QByteArray separator;
    qint64 expectedSize; // Expected size of incoming data
    bool sizeReceived;

    void synchronizeSetting(QByteArray setting, QByteArray pic);
    void CreateDir(QString dirname);
    void Login(QString usern, QString passw);
    void SearchUser(QString);

    void RenameFile(QString OldName, QString NewName);
    void RenameFolder(QString OldName, QString NewName);
    void CutFolder(QString from, QString to);
    void CopyFolder(QString from, QString to);
    void CutFile(QString from, QString to);
    void CopyFile(QString from, QString to);
    void Pause(QByteArray ptr,QByteArray booleen);
    void DeleteAccount();
    void DeleteFile(QString fileName);
    void SoftDeleteFile(QString fileName);
    void SoftDeleteFolder(QString fileName);
    void RestoreFile(QString fileName);
    void RestoreFolder(QString fileName);
    void DeleteFolder(QString fileName);

    void moveFile(QString fileName);
    void SignUp(QString usern, QString email, QString passw);
    void SignUpVerification(QString usern, QString email, QString passw,QString vNum);

    bool first=true;
    bool _init=false;
    bool _login=false;
    bool _renamefile=false;
    bool _renamefolder=false;
    bool _signup=false;
    bool _signupVerif=false;
    bool _deletefile=false;
    bool _deletefolder=false;
    bool _moveToDeleted=false;
    bool _synchronizeSetting=false;
    bool _createDir=false;
    bool _searchuser=false;
    bool _paused=false;
    bool _cutFolder=false;
    bool _copyFolder=false;
    bool _cutFile=false;
    bool _copyFile=false;
    bool _softDeleteFile=false;
    bool _softDeleteFolder=false;
    bool _restoreFile=false;
    bool _restoreFolder=false;
    bool _deleteAccount=false;

    void sendEncryptedData(const QByteArray &data);
    void processData();
    void processSearchData();
    void processDataHelper();
};

#endif // NetworkAgent_H
