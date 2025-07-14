#include "communicationdispatcher.h"
#include "networkagent.h"
#include <QCoreApplication>
#include <QTimer>
#include "tfileinfo.hpp"
#include "tcloud.hpp"

QString CommunicationDispatcher::m_lastError;

CommunicationDispatcher::CommunicationDispatcher(NetworkAgent* agent, QObject *parent)
    : QObject(parent),
    m_agent(agent),
    m_stdout(stdout)
{
    // Connect NetworkAgent signals to handle responses
    connect(m_agent, &NetworkAgent::loginSuccess, this, [this]() {
        m_stdout << "Login successful\n";
        //m_agent->userInit();
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::loginFailed, this, [this](const QString& error) {
        m_stdout << "Login failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::registerSuccess, this, [this]() {
        m_stdout << "Registration successful\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::registerFailed, this, [this](const QString& error) {
        m_stdout << "Registration failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::registerContinue, this, [this]() {
        m_stdout << "Please check your email for verification code\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::renameSuccess, this, [this]() {
        m_stdout << "Rename successful\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::renameFailed, this, [this](const QString& error) {
        m_stdout << "Rename failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::copySuccess, this, [this]() {
        m_stdout << "Copy operation successful\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::copyFailed, this, [this](const QString& error) {
        m_stdout << "Copy failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::cutSuccess, this, [this]() {
        m_stdout << "Cut operation successful\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::cutFailed, this, [this](const QString& error) {
        m_stdout << "Cut failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::createDirSuccess, this, [this]() {
        m_stdout << "Directory created successfully\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::createDirFailed, this, [this](const QString& error) {
        m_stdout << "Directory creation failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::deleteSuccess, this, [this]() {
        m_stdout << "Delete successful\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::deleteFailed, this, [this](const QString& error) {
        m_stdout << "Delete failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::restoreSuccess, this, [this]() {
        m_stdout << "Restore successful\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::restoreFailed, this, [this](const QString& error) {
        m_stdout << "Restore failed: " << error << "\n";
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::searchDone, this, [this](const QList<userData>& users) {
        m_stdout << "Search results:\n";
        for (const auto& user : users) {
            m_stdout << "  Username: " << user.m_name << "\n";
        }
        m_stdout.flush();
    });

    connect(m_agent, &NetworkAgent::initSuccess, this, [this]() {
        m_stdout << "Initialization successful\n";
        m_stdout.flush();
    });

    m_stdinNotifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(m_stdinNotifier, &QSocketNotifier::activated, this, &CommunicationDispatcher::handleStdin);
}

CommunicationDispatcher::~CommunicationDispatcher()
{
    delete m_stdinNotifier;
}

void CommunicationDispatcher::handleStdin()
{
    QTextStream in(stdin);
    QString line = in.readLine().trimmed();

    if (line.isEmpty()) {
        return;
    }

    emit commandReceived(line);
    handleCommand(line);
}

void CommunicationDispatcher::handleCommand(const QString& command)
{
    QStringList parts = command.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    QString cmd = parts[0].toLower();

    if (cmd == "help") {
        printHelp();
    }
    else if (cmd == "list") {
        QList<TFileInfo> files = m_agent->getList();
        m_stdout << "Files:\n";
        for (const auto& file : std::as_const(files)) {
            m_stdout << "  " << file.filepath
                     << " (" << TCLOUD::formatSize(file.size)
                     << " - " << file.uploadDate.toString(Qt::ISODate)
                     << (file.deleted ? " [DELETED]" : "") << "\n";
        }
        m_stdout.flush();
    }
    else if (cmd == "login" && parts.size() >= 3) {
        m_agent->login(parts[1], parts[2]);
    }
    else if (cmd == "register" && parts.size() >= 4) {
        m_agent->signUp(parts[1], parts[2], parts[3]);
    }
    else if (cmd == "verify" && parts.size() >= 5) {
        m_agent->signUpConfirm(parts[1], parts[2], parts[3], parts[4]);
    }
    else if (cmd == "rename" && parts.size() >= 3) {
        if (parts.size() > 3 && parts[1] == "file") {
            m_agent->renameFile(parts[2], parts[3]);
        }
        else if (parts.size() > 3 && parts[1] == "folder") {
            m_agent->renameFolder(parts[2], parts[3]);
        }
        else {
            m_stdout << "Usage: rename file|folder <oldname> <newname>\n";
            m_stdout.flush();
        }
    }
    else if (cmd == "copy" && parts.size() >= 3) {
        if (parts.size() > 3 && parts[1] == "file") {
            m_agent->copyFile(parts[2], parts[3]);
        }
        else if (parts.size() > 3 && parts[1] == "folder") {
            m_agent->copyFolder(parts[2], parts[3]);
        }
        else {
            m_stdout << "Usage: copy file|folder <source> <destination>\n";
            m_stdout.flush();
        }
    }
    else if (cmd == "cut" && parts.size() >= 3) {
        if (parts.size() > 3 && parts[1] == "file") {
            m_agent->cutFile(parts[2], parts[3]);
        }
        else if (parts.size() > 3 && parts[1] == "folder") {
            m_agent->cutFolder(parts[2], parts[3]);
        }
        else {
            m_stdout << "Usage: cut file|folder <source> <destination>\n";
            m_stdout.flush();
        }
    }
    else if (cmd == "mkdir" && parts.size() >= 2) {
        m_agent->createDir(parts[1]);
    }
    else if (cmd == "delete" && parts.size() >= 2) {
        if (parts.size() > 2 && parts[1] == "file") {
            m_agent->deleteFile(parts[2]);
        }
        else if (parts.size() > 2 && parts[1] == "folder") {
            m_agent->deleteFolder(parts[2]);
        }
        else {
            m_stdout << "Usage: delete file|folder <name>\n";
            m_stdout.flush();
        }
    }
    else if (cmd == "restore" && parts.size() >= 2) {
        if (parts.size() > 2 && parts[1] == "file") {
            m_agent->restoreFile(parts[2]);
        }
        else if (parts.size() > 2 && parts[1] == "folder") {
            m_agent->restoreFolder(parts[2]);
        }
        else {
            m_stdout << "Usage: restore file|folder <name>\n";
            m_stdout.flush();
        }
    }
    else if (cmd == "search" && parts.size() >= 2) {
        m_agent->searchUser(parts[1]);
    }
    else if (cmd == "space") {
        m_stdout << "Available space: " << m_agent->getAvailableSpace() << " bytes\n";
        m_stdout.flush();
    }
    else if (cmd == "quit" || cmd == "q") {
        m_agent->stop();
        QCoreApplication::quit();
    }
    else if (cmd == "init") {
        m_agent->userInit();
    }
    else if (cmd == "refresh") {
        m_agent->_refresh();
    }
    else {
        m_stdout << "Unknown command. Type 'help' for available commands.\n";
        m_stdout.flush();
    }
}

void CommunicationDispatcher::printHelp()
{
    m_stdout << "Available commands:\n"
             << "  help - Show this help message\n"
             << "  list - List all files\n"
             << "  login <username> <password> - Login to the system\n"
             << "  register <username> <email> <password> - Register a new account\n"
             << "  verify <username> <email> <password> <code> - Verify registration\n"
             << "  rename file|folder <oldname> <newname> - Rename a file or folder\n"
             << "  copy file|folder <source> <destination> - Copy a file or folder\n"
             << "  cut file|folder <source> <destination> - Move a file or folder\n"
             << "  mkdir <name> - Create a new directory\n"
             << "  delete file|folder <name> - Permanently delete a file or folder\n"
             << "  restore file|folder <name> - Restore from trash\n"
             << "  search <username> - Search for a user\n"
             << "  space - Show available storage space\n"
             << "  init - Initialize client data\n"
             << "  refresh - Refresh the UI\n"
             << "  quit/q - Quit the application\n";
    m_stdout.flush();
}

CommunicationDispatcher::MessageType CommunicationDispatcher::dispatchMessage(const QString& message)
{
    QStringList data= message.split("\n");
    if(data[0]=="INIT OK"){
        return INIT_OK;
    }else if(data[0]=="LOGIN OK"){
        return LOGIN_OK;

    }else if (data[0]=="LOGIN FAILED"){
        m_lastError=data[1];
        return LOGIN_FAILED;
    }else if (data[0]=="REGISTER FAILED"){
        m_lastError=data[1];
        return REGISTER_FAILED;
    }else if (data[0]=="REGISTER CONTINUE"){
        return REGISTER_CONTINUE;

    }else if(data[0]=="REGISTER OK"){
        return REGISTER_OK;
    }
    else if(data[0]=="DELETE FAILED"){
        m_lastError=data[1];
        return DELETE_FAILED;
    }
    else if(data[0]=="DELETE OK"){
        return DELETE_OK;
    }
    else if(data[0]=="RENAME FAILED"){
        m_lastError=data[1];
        return RENAME_FAILED;
    }
    else if(data[0]=="RENAME OK"){
        return RENAME_OK;
    }
    else if(data[0]=="SOFTDELETE FAILED"){
        return SOFTDELETE_FAILED;
    }
    else if(data[0]=="SOFTDELETE OK"){
        return SOFTDELETE_OK;
    }
    else if(data[0]=="CREATEDIR FAILED"){

        m_lastError=data[1];
        return CREATEDIR_FAILED;
    }
    else if(data[0]=="CREATEDIR OK"){
        return CREATEDIR_OK;
    }
    else if(data[0]=="SEARCH OK"){
        return SEARCH_OK;
    }
    else if(data[0]=="COPY OK"){
        return COPY_OK;
    }
    else if(data[0]=="CUT OK"){
        return CUT_OK;
    }
    else if(data[0]=="COPY FAILED"){

        m_lastError=data[1];
        return COPY_FAILED;
    }
    else if(data[0]=="CUT FAILED"){
        m_lastError=data[1];
        return CUT_FAILED;

    }
    else if(data[0]=="RESTORE OK"){
        return RESTORE_OK;
    }
    else if(data[0]=="RESTORE FAILED"){
        m_lastError=data[1];
        return RESTORE_FAILED;
    }
    else if(data[0]=="SYNCHRONIZESETTINGS FAILED"){
        m_lastError=data[1];
        return SYNCHRONIZESETTINGS_FAILED;
    }
    else if(data[0]=="SYNCHRONIZESETTINGS SUCCESS"){
        m_lastError=data[1];
        return SYNCHRONIZESETTINGS_OK;
    }
    else if(data[0]=="DELETEACCOUNT FAILED"){
        m_lastError=data[1];
        return DELETEACCOUNT_FAILED;
    }
    else if(data[0]=="DELETEACCOUNT OK"){
        m_lastError=data[1];
        return DELETEACCOUNT_OK;
    }

    return UNKNOWN;
}
