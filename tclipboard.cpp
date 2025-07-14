#include "tclipboard.h"
#include "tfolder.h"
#include "tfile.h"
#include "networkagent.h"


// tclipboard.cpp
#include "tclipboard.h"

TClipBoard* TClipBoard::s_instance = nullptr;

TClipBoard* TClipBoard::instance() {
    return s_instance;
}
void TClipBoard::Init() {
    if (!s_instance)
        s_instance = new TClipBoard();
}

void TClipBoard::cleanUp()
{
    if(s_instance){
        delete s_instance;
    }
}

TClipBoard::TClipBoard() : QObject(nullptr) {}

TClipBoard::~TClipBoard()
{
}

void TClipBoard::set(TransactionType type, const QList<eltCore>& items) {
    m_type = type;
    m_items = items;
}

void TClipBoard::set(TransactionType type, const eltCore &singleItem)
{
    m_type = type;
    m_items = { singleItem };
}

void TClipBoard::preparePaste() {
    m_rootOps.clear();
    for (const eltCore& elt : std::as_const(m_items)) {
        // folders: marked with trailing /. (top-level only)
        if (elt.type == TCloudElt::Folder && elt.name.endsWith("/.") && elt.root) {
            m_rootOps.append(elt);
        }
        // files: top-level only (no subdir in logical name)
        else if (elt.type == TCloudElt::File && !elt.name.contains('/')) {
            m_rootOps.append(elt);
        }
    }
}

void TClipBoard::clear() {
    m_type = None;
    m_items.clear();
    m_rootOps.clear();
    m_targetFolder = nullptr;
    m_agent = nullptr;
    m_pendingCount = 0;
}

void TClipBoard::pasteTo(TFolder* targetFolder, NetworkAgent* agent) {
    if (m_type == None || m_items.isEmpty()) return;

    preparePaste();

    m_agent = agent;
    m_targetFolder = targetFolder;
    m_targetPath = targetFolder->path();
    m_pendingCount = m_rootOps.size();

    connectSignals();

    for (const eltCore& elt : std::as_const(m_rootOps)) {
        const QString& logicalPath = elt.name;
        const QString& realPath = elt.path;
        const TCloudElt::Type fileType = elt.type;

        if (m_type == SoftDelete) {
            if (fileType ==TCloudElt::File) m_agent->softDeleteFolder(realPath);
            else m_agent->softDeleteFile(realPath);
            continue;
        }

        if (fileType == TCloudElt::Folder && logicalPath.endsWith("/.") ) {
            qDebug() << "realpath="<<realPath<<" m_targetPath="<<m_targetPath;
            m_agent->copyFolder(realPath, m_targetPath);
        } else if(fileType == TCloudElt::File) {
            m_agent->copyFile(realPath, m_targetPath+"/"+elt.name);
        }
    }
}

void TClipBoard::connectSignals() {
    disconnect(m_agent, nullptr, this, nullptr);

    connect(m_agent, &NetworkAgent::copySuccess, this, &TClipBoard::handleCopySuccess);
    connect(m_agent, &NetworkAgent::copyFailed, this, &TClipBoard::handleCopyFailed);
    connect(m_agent, &NetworkAgent::cutSuccess, this, &TClipBoard::handleCutSuccess);
    connect(m_agent, &NetworkAgent::cutFailed, this, &TClipBoard::handleCutFailed);
    connect(m_agent, &NetworkAgent::deleteSuccess, this, &TClipBoard::handleDeleteSuccess);
    connect(m_agent, &NetworkAgent::deleteFailed, this, &TClipBoard::handleDeleteFailed);
}

void TClipBoard::handleCopySuccess() {
    if (--m_pendingCount == 0) {
        emit operationDone(true, "Copy completed");
        clear();
    }
}

void TClipBoard::handleCopyFailed(QString error) {
    emit operationDone(false, "Copy failed: " + error);
    clear();
}

void TClipBoard::handleCutSuccess() {
    if (--m_pendingCount == 0) {
        emit operationDone(true, "Cut completed");
        clear();
    }
}

void TClipBoard::handleCutFailed(QString error) {
    emit operationDone(false, "Cut failed: " + error);
    clear();
}

void TClipBoard::handleDeleteSuccess() {
    if (--m_pendingCount == 0) {
        emit operationDone(true, "Move to trash completed");
        clear();
    }
}

void TClipBoard::handleDeleteFailed(QString error) {
    emit operationDone(false, "Move to trash failed: " + error);
    clear();
}
