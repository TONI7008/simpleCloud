#include <QMessageBox>
#include <QFileIconProvider>

#include "tfolder.h"
#include "tfile.h"
#include "tlabel.h"
#include "tpushbutton.h"
#include "tfilewidget.h"
#include "infopage.h"
#include "loader.h"
#include "tmenu.h"
#include "inputdialog.h"
#include "threadmanager.h"
#include "mainthread.h"
#include "tclipboard.h"

TFile::TFile(TFolder* folder,TFileInfo info,QWidget* parent) : TCloudElt(parent),
    m_pFolder(folder){

    m_info=info;
    m_name=extractFilename(info.filepath);
    m_type=TCloudElt::File;
    m_size=m_info.size;

    m_displayLayout=new QGridLayout(this);
    m_displayLayout->setContentsMargins(3,3,3,3);
    m_displayLayout->setVerticalSpacing(1);

    m_labelName=new TLabel(this);
    m_labelName->setObjectName("m_labelName");
    m_tFrame=new TFrame(this);
    m_tFrame->setCornerStyle(TFrame::CornerStyle::Default);
    m_load=new Loader(m_tFrame);

    m_labelName->setText(m_name);
    m_labelName->setAlignment(Qt::AlignHCenter);
    m_labelName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_labelName->setMinimumSize(162,70);


    m_icon=getIcon();
    m_iconButton = new TPushButton(this);
    m_iconButton->setIcon(m_icon);
    iconSize=QSize(100,80);
    m_iconButton->setFixedSize(iconSize);
    m_iconButton->setIconSize(iconSize);
    m_iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    m_displayLayout->addWidget(m_iconButton,0,0,Qt::AlignCenter);
    m_displayLayout->addWidget(m_labelName,1,0,Qt::AlignCenter);
    setLayout(m_displayLayout);


    setStyle(false);

    connect(this,&TFile::selectedChanged,this,[this](bool s){
        setStyle(s);
        update();
    });

    setBaseSize(170,155);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setEnableBackground(false);
    setEnableBackground(false);

    settingUpSignals();
    settingUpMenu();

}

TFile::~TFile()
{
    delete m_tFrame;
    if(contextMenu) delete contextMenu;
    if(m_deletedMenu) delete m_deletedMenu;
    m_load->deleteLater();

}
void TFile::settingUpSignals()
{
    m_labelName->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_iconButton->setAttribute(Qt::WA_TransparentForMouseEvents);

    connect(this, &TFrame::resizing, this, [this]{
        m_tFrame->setGeometry(contentsRect());
    });
}
void TFile::open(){
    m_pFolder->getThreadManager()->openFile(m_info.filepath);
}

void TFile::renamFile()
{
    InputDialog* inpD=InputDialog::instance();
    inpD->setPurpose(InputDialog::Purpose::Rename);
    *inpD << filePath()<<name()<<QString::number(0)<<QString::number(0);

    inpD->show();

    QEventLoop loop;
    connect(inpD, &InputDialog::DoneRenaming,&loop,[this,&loop](const QString& str){
        m_labelName->setText(str);
        m_labelName->update();
        loop.quit();
    });
    connect(inpD, &InputDialog::canceling,&loop,&QEventLoop::quit);
    loop.exec();
}

void TFile::deleteFile()
{
    m_pFolder->getMainThread()->deleteFile(m_info.filepath);

    connect(m_pFolder->getMainThread(), &mainThread::deleteSuccess, this, [this]{
        disconnect(m_pFolder->getMainThread(), &mainThread::deleteSuccess, nullptr, nullptr);
        m_pFolder->remove(this);
    });

    connect(m_pFolder->getMainThread(), &mainThread::deleteFailed, this, [this](QString error){
        disconnect(m_pFolder->getMainThread(), &mainThread::deleteFailed, nullptr, nullptr);
        qDebug() << "Error=" <<error;
    });
}

void TFile::moveToTrash()
{
    m_pFolder->getMainThread()->softDeleteFile(m_info.filepath);

    connect(m_pFolder->getMainThread(), &mainThread::deleteSuccess, this, [this]{
        disconnect(m_pFolder->getMainThread(), &mainThread::deleteSuccess, nullptr, nullptr);
        m_pFolder->softRemove(this);
    });

    connect(m_pFolder->getMainThread(), &mainThread::deleteFailed, this, [this](QString error){
        disconnect(m_pFolder->getMainThread(), &mainThread::deleteFailed, nullptr, nullptr);
        qDebug() << "Error=" <<error;
    });


}

void TFile::downloadFile()
{
    if(isDownloading){
        m_load->resume();
        return;
    }
    if(!downloaded){
        m_pFolder->getThreadManager()->download(m_info.filepath,m_pFolder->getMainThread()->getUsername(),m_load);
        isDownloading=true;
    }
}

void TFile::openFile()
{
    if(!downloaded){
        if(!isDownloading){
            downloadFile();
        }
        canOpen=true;
    }else{
        open();
    }
}

void TFile::restoreFile()
{
    m_pFolder->getMainThread()->restoreFile(m_info.filepath);

    connect(m_pFolder->getMainThread(), &mainThread::restoreSuccess, this, [this]{
        disconnect(m_pFolder->getMainThread(), &mainThread::restoreSuccess, nullptr, nullptr);
        m_pFolder->remove(this);
    });

    connect(m_pFolder->getMainThread(), &mainThread::restoreFailed, this, [this](QString error){
        disconnect(m_pFolder->getMainThread(), &mainThread::restoreFailed, nullptr, nullptr);
        qDebug() << "Error=" <<error;
    });
}


void TFile::settingUpMenu()
{
    bool deleted=m_info.deleted;
    if(deleted){
        m_deletedMenu=new TMenu(this);
        QAction* cutAction = new QAction(QIcon(":/icons/cutwhite.svg"),"Cut", m_deletedMenu);
        QAction* restoreAction = new QAction(QIcon(":/icons/restorewhite.svg"),"Restore", m_deletedMenu);
        QAction* deleteAction = new QAction(QIcon(":/icons/deletewhite.svg"),"delete", m_deletedMenu);

        m_deletedMenu->setBorderRadius(5);
        m_deletedMenu->setBorderSize(3);
        m_deletedMenu->setBorder(true);

        m_deletedMenu->addAction(cutAction);
        m_deletedMenu->addAction(restoreAction);
        m_deletedMenu->addAction(deleteAction);

        connect(deleteAction, &QAction::triggered, this,&TFile::deleteFile);
        connect(restoreAction, &QAction::triggered, this,&TFile::restoreFile);

    }else{
        contextMenu=new TMenu(this);

        contextMenu->setBorderRadius(5);
        contextMenu->setBorderSize(3);
        contextMenu->setBorder(true);

        QAction* copyAction = new QAction(QIcon(":/icons/copywhite.svg"),"Copy", contextMenu);
        QAction *cutAction = new QAction(QIcon(":/icons/cutwhite.svg"),"Cut", contextMenu);

        QAction* renameAction = new QAction(QIcon(":/icons/editwhite.svg"),"Rename", contextMenu);
        QAction* propertyAction = new QAction(QIcon(":/icons/infowhite.svg"),"Info", contextMenu);

        QAction* deleteAction = new QAction(QIcon(":/icons/deletewhite.svg"),"Move to trash", contextMenu);
        QAction* downloadAction = new QAction(QIcon(":/icons/downloadwhite.svg"),"Download", contextMenu);
        QAction* shareAction = new QAction(QIcon(":/icons/sharefilewhite.svg"),"Share", contextMenu);
        QAction* openAction = new QAction(QIcon(":/icons/openwhite.svg"),"Open", contextMenu);
        QAction* openDFolderAction = new QAction(QIcon(":/icons/openwhite.svg"),"Open file location", this);
        openDFolderAction->setEnabled(false);

        contextMenu->addAction(copyAction);
        contextMenu->addAction(cutAction);
        contextMenu->addAction(renameAction);
        contextMenu->addAction(propertyAction);
        contextMenu->addAction(deleteAction);
        contextMenu->addAction(downloadAction);
        contextMenu->addAction(shareAction);
        contextMenu->addAction(openAction);
        contextMenu->addAction(openDFolderAction);

        connect(renameAction, &QAction::triggered, this,&TFile::renamFile);

        connect(propertyAction, &QAction::triggered, this, [this]{
            InfoPage::instance()->showForFile(m_info);
        });

        connect(downloadAction, &QAction::triggered,this,&TFile::downloadFile);

        connect(openAction, &QAction::triggered,this,&TFile::openFile);

        connect(deleteAction, &QAction::triggered, this,&TFile::moveToTrash);


        connect(copyAction, &QAction::triggered,this,&TFile::copy);
        connect(cutAction, &QAction::triggered,this,&TFile::cut);

        connect(m_load,&Loader::done,this,[this]{
            isDownloading=false;
            downloaded=true;
            if(canOpen) open();

            qDebug("we received the loader done signal !");
        });

    }

    connect(this, &TFile::rightClicked, this, [this](const QPoint& pos) {
        if(!m_multiSelection){
            if(m_info.deleted){
                m_deletedMenu->exec(mapToGlobal(pos));
            }else{
                contextMenu->exec(mapToGlobal(pos));
            }
        }else if(m_fMenu) {
            m_fMenu->exec(mapToGlobal(pos));
        }
    });

}
eltCore TFile::core()
{
    return eltCore(name(),filePath(),m_info);
}

QString TFile::filePath(){
    return m_pFolder ? m_pFolder->path() + "/" +filename() : ":";;
}

void TFile::setStyle(bool s_style)
{
    if(s_style) setStyleSheet(selectedStyle);
    else setStyleSheet(initialStyle);
}

TFile *TFile::_copy()
{
    TFile *file = new TFile(nullptr,info(),nullptr);
    file->m_name=m_name;
    file->m_size=m_size;

    return file;
}

TFolder *TFile::parentFolder()
{
    return m_pFolder;
}

void TFile::setParentFolder(TFolder *p)
{
    if(!p) return;
    m_pFolder=p;
    setParent(m_pFolder->fileWidget());
}

void TFile::setInfo(TFileInfo inf)
{
    m_info=inf;
}

Loader *TFile::loader() const
{
    return m_load;
}

QWidget *TFile::originParent()
{
    return parentFolder()->originParent();
}

void TFile::copy()
{
    TClipBoard::instance()->append(core());
    TClipBoard::instance()->setType(TClipBoard::Copy);
}
void TFile::cut()
{
    TClipBoard::instance()->setType(TClipBoard::Cut);
    TClipBoard::instance()->append(core());

    m_pFolder->remove(this);
}


QString TFile::extractFilename(QString path)
{
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    return parts.isEmpty() ? "Unknown" : parts.last();
}

QIcon TFile::getIcon(){
    QFileIconProvider provider;
    QFileInfo inf(m_name);
    return provider.icon(inf);
}
