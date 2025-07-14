#include "tfolder.h"
#include <QIcon>
#include <QDebug>
#include <QMessageBox>

#include "tfile.h"
#include "tlabel.h"
#include "tmenu.h"
#include "tfilewidget.h"
#include "networkagent.h"
#include "infopage.h"
#include "inputdialog.h"
#include "tclipboard.h"
#include "tstackedwidget.h"
#include "tnotification.h"

// Initialize static TFolder registry
QMap<QString, TFolder*> TFolder::FolderRegistry;

TFolder* TFolder::m_currentFolder=nullptr;

QMap<QString, TFolder*> TFolder::DFolderRegistry;

QMap<QString, TFolder*> TFolder::SFolderRegistry;


int TFolder::currentFolderIndex = -1;
TStackedWidget* TFolder::displayStackWidget;

// Constructor
TFolder::TFolder(TFileInfo info, bool base,TFolder* pTFolder,ThreadManager* t,NetworkAgent* mt,TFileManager* fm,QWidget *parent)
    : TCloudElt(parent),m_mThread(mt),m_Tmanager(t),
    m_fManager(fm),
    m_pTFolder(pTFolder),m_base(base)
{
    // Extract TFolder name and set path
    m_info=info;
    m_name = extractFolderName(m_info.filepath);
    m_type=TCloudElt::Folder;
    m_displayLayout = new QGridLayout(this);

    // Set the object name
    setObjectName(m_name);


    // Scroll Area Setup
    setupScrollArea();

    // If not the base TFolder, create a TFolder icon view
    if (!base) {
        setupFolderIconView();
    }else{
        hide();
    }

    // File view setup
    m_fileWidget=new TFileWidget(this,m_fManager);

    m_scrollArea->setWidget(m_fileWidget);

    if (!base) {
        connect(this, &TCloudElt::doubleClicked, this, &TFolder::openFolder);


        setStyle(false);
        setEnableBackground(false);
        setEnableBackground(false);
    }

    setLayout(m_displayLayout);

}

TFolder::~TFolder()
{
    m_WfileList.clear();
    if(contextMenu) delete contextMenu;
    if(m_deletedMenu) delete m_deletedMenu;
}

void TFolder::settingUpMenu()
{
    bool deleted=m_info.deleted;
    if(deleted){
        m_deletedMenu=new TMenu(this);
        QAction* cutAction = new QAction(QIcon(":/icons/cutwhite.svg"),"Cut", m_deletedMenu);
        QAction* restoreAction = new QAction(QIcon(":/icons/restorewhite.svg"),"Restore", m_deletedMenu);
        QAction* deleteAction = new QAction(QIcon(":/icons/trashwhite.svg"),"delete", m_deletedMenu);

        m_deletedMenu->setBorderRadius(5);
        m_deletedMenu->setBorderSize(3);
        m_deletedMenu->setBorder(true);

        m_deletedMenu->addAction(cutAction);
        m_deletedMenu->addAction(restoreAction);
        m_deletedMenu->addAction(deleteAction);

        connect(deleteAction, &QAction::triggered, this,&TFolder::deleteFolder);
        connect(restoreAction, &QAction::triggered, this,&TFolder::restoreFolder);

    }else{
        contextMenu=new TMenu(this);

        contextMenu->setBorderRadius(5);
        contextMenu->setBorder(true);
        contextMenu->setBorderSize(3);

        QAction* copyAction = new QAction(QIcon(":/icons/copywhite.svg"),"Copy", this);
        QAction* cutAction = new QAction(QIcon(":/icons/cutwhite.svg"),"Cut", this);

        QAction* renameAction = new QAction(QIcon(":/icons/editwhite.svg"),"Rename", this);
        QAction* propertyAction = new QAction(QIcon(":/icons/infowhite.svg"),"Info", this);
        QAction* addBookmarkAction = new QAction(QIcon(":/icons/favoritewhite.svg"),"Add to favorite", this);

        QAction* deleteAction = new QAction(QIcon(":/icons/deletewhite.svg"),"Move to trash", this);
        QAction* downloadAction = new QAction(QIcon(":/icons/downloadwhite.svg"),"Download", this);
        QAction* openAction = new QAction(QIcon(":/icons/openwhite.svg"),"Open", this);

        contextMenu->addAction(openAction);
        contextMenu->addAction(copyAction);
        contextMenu->addAction(cutAction);
        contextMenu->addAction(renameAction);
        contextMenu->addAction(downloadAction);
        contextMenu->addAction(addBookmarkAction);
        contextMenu->addSeparator();
        contextMenu->addAction(deleteAction);
        contextMenu->addSeparator();
        contextMenu->addAction(propertyAction);

        connect(renameAction, &QAction::triggered, this,&TFolder::handleRename);

        connect(propertyAction, &QAction::triggered, this,[&]{
            InfoPage::instance()->showForFolder(m_info);
        });

        connect(downloadAction, &QAction::triggered,this,[&]{
            if(!downloaded){
                downloaded=true;
            }
        });

        connect(openAction, &QAction::triggered,this,[&]{
            //setFileView();
        });

        connect(deleteAction, &QAction::triggered, this,&TFolder::moveToTrash);



        connect(copyAction, &QAction::triggered,this,[this]{
            copyFolder();
        });

        connect(cutAction, &QAction::triggered,this,[this]{
            cutFolder();
        });
        connect(addBookmarkAction, &QAction::triggered,this,[this]{
            if(!hasBookmark){
                TFolderLink* link= new TFolderLink(m_info,m_favoriteLayout,m_fManager);

                connect(link,&TFolderLink::aboutToBeRemoved,this,[this]{
                    hasBookmark=false;
                });

                hasBookmark=true;
            }
        });

    }

    connect(this, &TCloudElt::rightClicked, this,&TFolder::handleRightClicked);
}

void TFolder::openFolder()
{
    if(m_currentFolder==this){
        if(m_scrollArea!=displayStackWidget->currentWidget()){
            displayStackWidget->setCurrentWidget(m_scrollArea);
            m_fManager->setPath(path());
        }
        return;
    }
    m_currentFolder=this;
    displayStackWidget->setCurrentWidget(m_scrollArea);
    m_fManager->setPath(path());
}

void TFolder::openPreviousFolder()
{
    if(m_pTFolder){
        m_pTFolder->openFolder();
    }
}

void TFolder::add(TCloudElt* cloudElt)
{
    if (!cloudElt) {
        qWarning() << "Cannot add a null elt.";
        return;
    }
    if(cloudElt->type()==TCloudElt::File){
        TFile* file=qobject_cast<TFile*>(cloudElt);
        if (!file) {
            qWarning() << "Cannot add a null file.";
            return;
        }
        file->setParentFolder(this);
    }else if(cloudElt->type()==TCloudElt::Folder){

        TFolder* folder=qobject_cast<TFolder*>(cloudElt);
        if (!folder) {
            qWarning() << "Cannot add a null Folder.";
            return;
        }
        // Ensure no duplicate subTFolders
        if (!subFolders.contains(folder->name())) {
            subFolders[folder->name()] = folder;
            folder->setParentFolder(this);
        }
    }

    m_eltList.append(cloudElt->core());
    m_WfileList.append(cloudElt);
    m_fileWidget->add(cloudElt);
    updateInfo();
}

void TFolder::removeAll()
{
    for(auto elt : std::as_const(m_WfileList)){
        if(elt->type()==Folder){
            TFolder* folder=qobject_cast<TFolder*>(elt);
            folder->removeAll();
        }
    }

    subFolders.clear();
    m_WfileList.clear();
    m_fileWidget->Clear();
}

void TFolder::remove(TCloudElt *elt)
{
    if(elt->type()==Folder)  {
        subFolders.remove(elt->name());
        if(elt->info().deleted) {
            DFolderRegistry.remove(elt->name());
        }else{
            FolderRegistry.remove(elt->name());
        }
    };
    m_WfileList.removeOne(elt);
    m_fileWidget->remove(elt);
}


void TFolder::softRemove(TCloudElt *elt)
{
    if(elt->type()==Folder)  {
        subFolders.remove(elt->name());
        if(elt->info().deleted) {
            DFolderRegistry.remove(elt->name());
        }else{
            FolderRegistry.remove(elt->name());
        }
    };
    m_WfileList.removeOne(elt);
    m_fileWidget->softRemove(elt);
}


// Rename the TFolder
void TFolder::rename(QString newName)
{
    if (!newName.isEmpty()) {
        m_name = newName;
        if (m_labelName) {
            m_labelName->setText(m_name);
        }
    }
    updateInfo();
}
void TFolder::handleRename(){
    InputDialog* inpD=InputDialog::instance();
    inpD->setPurpose(InputDialog::Purpose::Rename);
    *inpD << path()<<name()<<QString::number(1)<<QString::number(m_WfileList.isEmpty());

    inpD->exec();

    QEventLoop loop;
    connect(inpD, &InputDialog::DoneRenaming,&loop,[this,&loop](const QString& str){
        rename(str);
        loop.quit();
    });
    connect(inpD, &InputDialog::canceling,&loop,&QEventLoop::quit);
    loop.exec();
}

void TFolder::moveToTrash()
{
    m_pTFolder->getNetworkAgent()->softDeleteFolder(m_info.filepath);

    // Disconnect previous connections to avoid duplicate message boxes
    disconnect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteSuccess, nullptr, nullptr);
    disconnect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteFailed, nullptr, nullptr);

    connect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteSuccess, this, [this]{
        m_pTFolder->remove(this);
        TNotifaction::instance()->setMessage("Success moving to trash folder " + name());

    });

    connect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteFailed, this, [this](QString error){
        TNotifaction::instance()->setMessage("Error"+error+" while moving to trash folder " + name(),true);
    });
}

void TFolder::deleteFolder()
{
    m_pTFolder->getNetworkAgent()->deleteFolder(m_info.filepath);

    // Disconnect previous connections to avoid duplicate message boxes
    disconnect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteSuccess, nullptr, nullptr);
    disconnect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteFailed, nullptr, nullptr);

    connect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteSuccess, this, [this]{
        m_pTFolder->remove(this);
        TNotifaction::instance()->setMessage("sucess deleting folder " + name());
    });

    connect(m_pTFolder->getNetworkAgent(), &NetworkAgent::deleteFailed, this, [this](QString error){
        TNotifaction::instance()->setMessage("Error"+error+" while deleting folder " + name(),false);
    });
}

void TFolder::restoreFolder()
{
    getNetworkAgent()->restoreFolder(m_info.filepath+"/.");

    connect(getNetworkAgent(), &NetworkAgent::restoreSuccess, this, [this]{
        disconnect(getNetworkAgent(), &NetworkAgent::restoreSuccess, nullptr, nullptr);
        parentFolder()->remove(this);
        TNotifaction::instance()->setMessage("Success restoring folder " + name());
    });

    connect(getNetworkAgent(), &NetworkAgent::restoreFailed, this, [this](QString error){
        disconnect(getNetworkAgent(), &NetworkAgent::restoreFailed, nullptr, nullptr);
        TNotifaction::instance()->setMessage("Error"+error+" while restoring folder " + name(),true);
    });
}


QString TFolder::path() const {
    return m_pTFolder ? m_pTFolder->path() + "/" + m_name : ":";
}

QList<eltCore> TFolder::getList() const
{

    QList<eltCore> tempList = m_eltList; // Initialize directly with m_WfileList

    for (const TFolder* f : subFolders) { // Use const TFolder* and const range
        tempList.append(f->getList());
    }
    return tempList;
}

TFolder* TFolder::parentFolder() { return m_pTFolder; }

void TFolder::setParentFolder(TFolder *p)
{
    if(!p) return;
    m_pTFolder=p;
    setParent(m_pTFolder->fileWidget());
}

TFileWidget* TFolder::fileWidget() { return m_fileWidget; }

// Organise path by removing current TFolder's name
QString TFolder::Organise(QString path)
{
    if (!m_base) {
        return path.replace(m_name + "/", "");
    } else {
        return path.replace(":/", "");
    }
}

// Extract first TFolder name from path
QString TFolder::extractFolderName(QString path)
{
    if (m_base) {
        return "";
    }
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    return parts.isEmpty() ? "" : parts.first();
}

// Setup scroll area
void TFolder::setupScrollArea()
{
    m_scrollArea = new QScrollArea(displayStackWidget);
    m_scrollArea->setObjectName("m_scrollArea");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setWindowFlag(Qt::FramelessWindowHint);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    displayStackWidget->addWidget(m_scrollArea);
}

// Setup TFolder icon view
void TFolder::setupFolderIconView()
{

    m_labelName = new TLabel(m_name, this);
    m_labelName->setObjectName("m_labelName");
    m_labelName->setAlignment(Qt::AlignHCenter);
    m_labelName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_labelName->setMinimumSize(QSize(162,70));

    m_icon = QIcon(":/icons/folder.svg");
    m_iconButton = new QPushButton(this);
    m_iconButton->setIcon(m_icon);
    m_iconButton->setIconSize(QSize(100,82));
    m_iconButton->setFixedSize(QSize(100,82));
    m_iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    //m_iconButton->setStyleSheet("background: transparent; border: none;");


    m_displayLayout->addWidget(m_iconButton,0,0,Qt::AlignCenter);
    m_displayLayout->addWidget(m_labelName,1,0,Qt::AlignCenter);
    m_displayLayout->setContentsMargins(3,2,4,2);
    m_displayLayout->setVerticalSpacing(1);

    m_labelName->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_iconButton->setAttribute(Qt::WA_TransparentForMouseEvents);


    settingUpMenu();

    setBaseSize(170,155);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(this,&TFolder::selectedChanged,this,[this](bool s){
        setStyle(s);
        update();
    });

}


void TFolder::updateInfo()
{
    if(!m_base){
        m_info.size=getSize();
        m_info.filepath=path();
        for (auto it = subFolders.constBegin(); it != subFolders.constEnd(); ++it) {
            it.value()->updateInfo();
        }
    }else{
        for (auto it = subFolders.constBegin(); it != subFolders.constEnd(); ++it) {
            it.value()->updateInfo();
        }
    }

}

eltCore TFolder::core()
{
    return eltCore(name(),path(),m_info,m_type);
}

void TFolder::copy()
{
    copyFolder();
}

void TFolder::cut()
{
    cutFolder();
}

void TFolder::deleteElt()
{

}


// Setup TFolder structure recursively
bool TFolder::setupFolder(TFolder* pFolder, TFileInfo finalInfo)
{
    if (finalInfo.filepath.contains("/")) {
        QStringList pathComponents = finalInfo.filepath.split("/", Qt::SkipEmptyParts);
        if (pathComponents.isEmpty()) return false;
        TFileInfo info=finalInfo;
        QString firstFolder = pathComponents.first();
        info.filepath=firstFolder;
        finalInfo.filepath = finalInfo.filepath.replace(firstFolder + "/", "");
        QString fullPath = pFolder->path().endsWith("/") ? pFolder->path() + firstFolder : pFolder->path() + "/" + firstFolder;
        bool contains=FolderRegistry.contains(fullPath);
        TFolder* currentFolder = contains ? FolderRegistry[fullPath] : new TFolder(info, false, pFolder,pFolder->m_Tmanager,pFolder->m_mThread,pFolder->m_fManager,pFolder->fileWidget());
        if (!contains) {
            FolderRegistry[fullPath] = currentFolder;
            pFolder->add(currentFolder);
        }
        //finalInfo.data();
        return setupFolder(currentFolder,finalInfo);
    } else {
        if (!finalInfo.filepath.isEmpty() && finalInfo.filepath != ".") {
            finalInfo.filepath = pFolder->path() + "/" + finalInfo.filepath;
            //finalInfo.data();
            TFile* file = new TFile(pFolder,finalInfo, pFolder->fileWidget());
            pFolder->add(file);
            return true;
        }
        return false;
    }
}


void TFolder::setupFolderAsync(TFolder* pFolder, TFileInfo finalInfo)
{
    QTimer::singleShot(10, 0,[pFolder, finalInfo]() mutable {
        if (finalInfo.filepath.contains("/")) {
            QStringList pathComponents = finalInfo.filepath.split("/", Qt::SkipEmptyParts);
            if (pathComponents.isEmpty()) return;

            TFileInfo info = finalInfo;
            QString firstFolder = pathComponents.first();
            info.filepath = firstFolder;

            finalInfo.filepath = finalInfo.filepath.mid(firstFolder.length() + 1);

            QString fullPath = pFolder->path().endsWith("/") ? pFolder->path() + firstFolder : pFolder->path() + "/" + firstFolder;

            TFolder* currentFolder = nullptr;
            if (FolderRegistry.contains(fullPath)) {
                currentFolder = FolderRegistry[fullPath];
            } else {
                currentFolder = new TFolder(info, false, pFolder, pFolder->m_Tmanager, pFolder->m_mThread, pFolder->m_fManager, pFolder->fileWidget());
                FolderRegistry[fullPath] = currentFolder;
                pFolder->add(currentFolder);
            }

            QTimer::singleShot(10,0, [currentFolder, finalInfo]() mutable {
                currentFolder->setupFolderAsync(currentFolder, finalInfo);
            });

        } else {
            if (!finalInfo.filepath.isEmpty() && finalInfo.filepath != ".") {
                finalInfo.filepath = pFolder->path() + "/" + finalInfo.filepath;
                TFile* file = new TFile(pFolder, finalInfo, pFolder->fileWidget());
                pFolder->add(file);
            }
        }
    });
}

bool TFolder::setupDeletedFolder(TFolder* pFolder, TFileInfo finalInfo)
{
    if (finalInfo.filepath.contains("/")) {
        QStringList pathComponents = finalInfo.filepath.split("/", Qt::SkipEmptyParts);
        if (pathComponents.isEmpty()) return false;
        TFileInfo info=finalInfo;
        QString firstFolder = pathComponents.first();
        info.filepath=firstFolder;
        finalInfo.filepath = finalInfo.filepath.replace(firstFolder + "/", "");
        QString fullPath = pFolder->path().endsWith("/") ? pFolder->path() + firstFolder : pFolder->path() + "/" + firstFolder;
        TFolder* currentFolder = DFolderRegistry.contains(fullPath) ? DFolderRegistry[fullPath] : new TFolder(info, false, pFolder,pFolder->m_Tmanager,pFolder->m_mThread,pFolder->m_fManager,pFolder->fileWidget());
        if (!DFolderRegistry.contains(fullPath)) {
            DFolderRegistry[fullPath] = currentFolder;
            pFolder->add(currentFolder);
        }
        return setupDeletedFolder(currentFolder,finalInfo);
    } else {
        if (!finalInfo.filepath.isEmpty() && finalInfo.filepath != ".") {
            finalInfo.filepath = pFolder->path() + "/" + finalInfo.filepath;
            TFile* file = new TFile(pFolder,finalInfo, pFolder->fileWidget());
            pFolder->add(file);
            return true;
        }
        return false;
    }
}



bool TFolder::setupSearchFolder(TFolder* pFolder, TFileInfo finalInfo)
{
    if (finalInfo.filepath.contains("/")) {
        QStringList pathComponents = finalInfo.filepath.split("/", Qt::SkipEmptyParts);
        if (pathComponents.isEmpty()) return false;
        TFileInfo info=finalInfo;
        QString firstFolder = pathComponents.first();
        info.filepath=firstFolder;
        finalInfo.filepath = finalInfo.filepath.replace(firstFolder + "/", "");
        QString fullPath = pFolder->path().endsWith("/") ? pFolder->path() + firstFolder : pFolder->path() + "/" + firstFolder;
        TFolder* currentFolder = SFolderRegistry.contains(fullPath) ? SFolderRegistry[fullPath] : new TFolder(info, false, pFolder,pFolder->m_Tmanager,pFolder->m_mThread,pFolder->m_fManager,pFolder->fileWidget());
        if (!SFolderRegistry.contains(fullPath)) {
            SFolderRegistry[fullPath] = currentFolder;
            pFolder->add(currentFolder);
        }
        return setupSearchFolder(currentFolder,finalInfo);
    } else {
        if (!finalInfo.filepath.isEmpty() && finalInfo.filepath != ".") {
            //finalInfo.filepath = pFolder->path() + "/" + finalInfo.filepath;
            TFile* file = new TFile(pFolder,finalInfo, pFolder->fileWidget());
            pFolder->add(file);
            return true;
        }
        return false;
    }
}

void TFolder::setupDeletedFolderAsync(TFolder* pFolder, TFileInfo finalInfo)
{
    QTimer::singleShot(20, 0,[pFolder, finalInfo]() mutable {
        if (finalInfo.filepath.contains("/")) {
            QStringList pathComponents = finalInfo.filepath.split("/", Qt::SkipEmptyParts);
            if (pathComponents.isEmpty()) return;

            TFileInfo info = finalInfo;
            QString firstFolder = pathComponents.first();
            info.filepath = firstFolder;

            finalInfo.filepath = finalInfo.filepath.mid(firstFolder.length() + 1);

            QString fullPath = pFolder->path().endsWith("/") ? pFolder->path() + firstFolder : pFolder->path() + "/" + firstFolder;

            TFolder* currentFolder = nullptr;
            if (DFolderRegistry.contains(fullPath)) {
                currentFolder = DFolderRegistry[fullPath];
            } else {
                currentFolder = new TFolder(info, false, pFolder, pFolder->m_Tmanager, pFolder->m_mThread, pFolder->m_fManager, pFolder->fileWidget());
                DFolderRegistry[fullPath] = currentFolder;
                pFolder->add(currentFolder);
            }

            QTimer::singleShot(20,0, [currentFolder, finalInfo]() mutable {
                currentFolder->setupDeletedFolderAsync(currentFolder, finalInfo);
            });

        } else {
            if (!finalInfo.filepath.isEmpty() && finalInfo.filepath != ".") {
                finalInfo.filepath = pFolder->path() + "/" + finalInfo.filepath;
                TFile* file = new TFile(pFolder, finalInfo, pFolder->fileWidget());
                pFolder->add(file);
            }
        }
    });
}

void TFolder::reset()
{
    FolderRegistry.clear();

    DFolderRegistry.clear();

    SFolderRegistry.clear();

    currentFolderIndex = -1;
}

TFolder *TFolder::currentFolder()
{
    return m_currentFolder;
}

void TFolder::setDisplayStackWidget(TStackedWidget *stackedW)
{
    displayStackWidget=stackedW;
}

// Set style
void TFolder::setStyle(bool s_style)
{
    setStyleSheet(s_style ? selectedStyleSheet : initialStyleSheet);
}


// Copy the TFolder
TFolder* TFolder::_copy()
{
    TFolder* newTFolder = new TFolder(m_info, m_base, m_pTFolder,m_Tmanager,m_mThread,m_fManager,parentWidget());
    newTFolder->setObjectName(objectName());
    newTFolder->setStyleSheet(styleSheet());

    // Copy files
    for (auto &file : m_WfileList) {
        TFile *f=qobject_cast<TFile*>(file);
        if(f){
            newTFolder->add(f->_copy());
        }
    }

    for (auto &subTFolder : subFolders) {
        newTFolder->add(subTFolder->_copy());
    }
    return newTFolder;
}

/*void TFolder::copyFolder(QString oldName)
{
    TClipBoard* clipBoard = TClipBoard::instance();
    if(!clipBoard) return;
    if(!multiSelection()) clipBoard->clear();

    clipBoard->setType(TClipBoard::Copy);
    eltCore core = this->core();
    if(oldName.isEmpty()) clipBoard->addElt(path(),type());

    // Build the base path for this folder and its contents
    QString basePath = oldName.isEmpty() ? core.name : oldName + "/" + core.name;
    core.name = basePath + "/.";  // Mark as folder with trailing /.
    //qDebug() << "folder name=" << core.name;
    clipBoard->append(core);

    // Process subfolders
    for(TFolder* elt : std::as_const(subFolders)) {
        elt->copyFolder(basePath);  // Pass the correct base path
    }

    // Process files
    for(TCloudElt* elt : std::as_const(m_WfileList)) {
        if(elt->type() == File) {
            eltCore fileCore = elt->core();
            fileCore.name = basePath + "/" + fileCore.name;  // Use the consistent base path
            //qDebug() << "file name=" << fileCore.name;
            clipBoard->append(fileCore);
        }
    }
}*/

void TFolder::copyFolder(QString oldName) {
    TClipBoard* clipBoard = TClipBoard::instance();
    if (!clipBoard) return;

    QList<eltCore> list;
    QString base = oldName.isEmpty() ? name() : oldName + "/" + name();

    eltCore rootCore = core();
    rootCore.name = base + "/.";
    rootCore.root=true;
    list.append(rootCore);

    for (TFolder* sub : std::as_const(subFolders))
        sub->collectClipboardItems(list, base);

    for (TCloudElt* elt : std::as_const(m_WfileList)) {
        if (elt->type() == TCloudElt::File) {
            eltCore fileCore = elt->core();
            fileCore.name = base + "/" + fileCore.name;
            list.append(fileCore);
        }
    }
    qDebug() << rootCore.name;
    qDebug() << list;

    clipBoard->set(TClipBoard::Copy, list);
}
void TFolder::collectClipboardItems(QList<eltCore>& out, const QString& base) {
    QString folderName = base + "/" + name();
    eltCore folderCore = core();
    folderCore.name = folderName + "/.";
    out.append(folderCore);

    for (TFolder* sub : std::as_const(subFolders))
        sub->collectClipboardItems(out, folderName);

    for (TCloudElt* elt : std::as_const(m_WfileList)) {
        if (elt->type() == TCloudElt::File) {
            eltCore fileCore = elt->core();
            fileCore.name = folderName + "/" + fileCore.name;
            out.append(fileCore);
        }
    }
}


void TFolder::cutFolder(QString oldName) {
    TClipBoard* clipBoard = TClipBoard::instance();
    if (!clipBoard) return;

    QList<eltCore> list;
    QString base = oldName.isEmpty() ? name() : oldName + "/" + name();

    eltCore rootCore = core();
    rootCore.name = base + "/.";
    list.append(rootCore);

    for (TFolder* sub : std::as_const(subFolders))
        sub->collectClipboardItems(list, base);

    for (TCloudElt* elt : std::as_const(m_WfileList)) {
        if (elt->type() == TCloudElt::File) {
            eltCore fileCore = elt->core();
            fileCore.name = base + "/" + fileCore.name;
            list.append(fileCore);
        }
    }

    clipBoard->set(TClipBoard::Cut, list);
    if (m_pTFolder) m_pTFolder->remove(this); // remove self from UI
}



qint64 TFolder::getSize(){

    qint64 size = 0;
    for (TCloudElt* elt : std::as_const(m_WfileList)) {
        if (elt->type()==File) {
            size += elt->size();
        }else if(elt->type()==Folder){
            TFolder* folder=qobject_cast<TFolder*>(elt);
            size+=folder->getSize();
        }
    }
    m_size=size;

    return size;
}

ThreadManager *TFolder::getThreadManager() const
{
    return m_Tmanager;
}

QWidget *TFolder::originParent()
{
    if(m_base){
        return parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget();
    }else{
        return parentFolder()->originParent();
    }
}

void TFolder::handleRightClicked(const QPoint &pos)
{
    if(!multiSelection()){
        if(m_info.deleted){
            m_deletedMenu->exec(mapToGlobal(pos));
        }else{
            contextMenu->exec(mapToGlobal(pos));
        }
    }else if(m_fMenu){
        m_fMenu->exec(mapToGlobal(pos));
    }
}

TFolder::SortType TFolder::sortType() const
{
    return m_sortType;
}

void TFolder::setSortType(SortType newSortType,bool order)
{
    if(newSortType==m_sortType) return;

    m_sortType = newSortType;

    for(auto folder : std::as_const(subFolders)){
        folder->setSortType(newSortType);
    }


    switch (m_sortType) {
    case Size:
        m_fileWidget->sortSize(order);
        break;
    case Name:
        m_fileWidget->sortName(order);
        break;
    case Type:
        m_fileWidget->sortType(order);
        break;
    default:
        break;
    }

}

QVBoxLayout *TFolder::favoriteLayout() const
{
    return m_favoriteLayout;
}

void TFolder::setFavoriteLayout(QVBoxLayout *newFavoriteLayout)
{
    if(!newFavoriteLayout) return;
    m_favoriteLayout = newFavoriteLayout;
    for(auto* folder : std::as_const(subFolders)){
        folder->setFavoriteLayout(m_favoriteLayout);
    }
}

NetworkAgent *TFolder::getNetworkAgent() const
{
    return m_mThread;
}


TFolderLink::TFolderLink(TFileInfo info,QVBoxLayout* layout, TFileManager *fm, QWidget *parent) :
    TCloudElt(parent),
    m_layout(layout)
{
    m_info=info;
    m_Manager=fm;

    m_icon=QIcon(":/icons/folder.svg");
    m_type=TCloudElt::Shorcut;

    m_name=extractName(info.filepath);

    m_displayLayout=new QGridLayout(this);
    m_displayLayout->setHorizontalSpacing(3);
    m_displayLayout->setContentsMargins(3,3,3,3);

    m_labelName=new TLabel(this);
    m_labelName->setObjectName("m_labelName");
    m_iconButton=new QPushButton(this);
    iconSize=QSize(24,24);
    m_iconButton->setIcon(m_icon);
    m_iconButton->setIconSize(iconSize);
    m_iconButton->setFixedSize(iconSize);
    m_labelName->setText(m_name);
    m_labelName->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_iconButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    m_displayLayout->addWidget(m_iconButton,0,0,Qt::AlignLeft);
    m_displayLayout->addWidget(m_labelName,0,1);

    connect(this,&TCloudElt::Clicked,this,&TFolderLink::gotoFolder);


    connectingSignals();

    setFixedHeight(45);

    setEnableBackground(false);
    setEnableBackground(false);

    setStyleSheet(initialStyleSheet);
    setWindowFlag(Qt::FramelessWindowHint);

    setLayout(m_displayLayout);

    m_layout->addWidget(this,pos);
    pos++;
}

TFolderLink::~TFolderLink()
{

}

QString TFolderLink::extractName(const QString &path)
{
    QString npath=path;
    npath.replace(":/","");
    QStringList parts = npath.split("/", Qt::SkipEmptyParts);
    return parts.isEmpty() ? " " : parts.last();
}

void TFolderLink::connectingSignals()
{
    m_labelName->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_iconButton->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void TFolderLink::gotoFolder()
{
    m_Manager->gotoFolder(m_info.filepath);
}
