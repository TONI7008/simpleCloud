#ifndef COMMUNICATIONDISPATCHER_H
#define COMMUNICATIONDISPATCHER_H

#include <QObject>
#include <QSocketNotifier>
#include <QTextStream>


class NetworkAgent;

class CommunicationDispatcher : public QObject
{
    Q_OBJECT
public:
    enum MessageType {
        SERVER_ALERT,
        UPDATE_NOTIFICATION,
        INIT_OK,
        LOGIN_OK,
        LOGIN_FAILED,
        REGISTER_OK,
        REGISTER_FAILED,
        REGISTER_CONTINUE,
        RENAME_OK,
        RENAME_FAILED,
        COPY_OK,
        COPY_FAILED,
        CUT_OK,
        CUT_FAILED,
        CREATEDIR_OK,
        CREATEDIR_FAILED,
        SOFTDELETE_OK,
        SOFTDELETE_FAILED,
        DELETE_OK,
        DELETE_FAILED,
        RESTORE_OK,
        RESTORE_FAILED,
        SEARCH_OK,
        SYNCHRONIZESETTINGS_OK,
        SYNCHRONIZESETTINGS_FAILED,
        DELETEACCOUNT_OK,
        DELETEACCOUNT_FAILED,
        UNKNOWN
    };

    Q_ENUM(MessageType);

    explicit CommunicationDispatcher(NetworkAgent* agent, QObject *parent = nullptr);
    ~CommunicationDispatcher();

    static MessageType dispatchMessage(const QString& message = QString());
    void handleCommand(const QString& command);

signals:
    void messageReceived(CommunicationDispatcher::MessageType type, const QString& message);
    void commandReceived(const QString& command);

private slots:
    void handleStdin();

private:
    void printHelp();

    QSocketNotifier* m_stdinNotifier;
    NetworkAgent* m_agent;
    QTextStream m_stdout;
    static QString m_lastError;
};

#endif // COMMUNICATIONDISPATCHER_H
