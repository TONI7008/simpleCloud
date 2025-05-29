#include "tfilemanager.h"
#include "tfolder.h"
#include "hoverbutton.h"
#include "tgridlayout.h"
#include "tfile.h"
#include "tfilewidget.h"
#include "tstackedwidget.h"
#include "tlineedit.hpp"

#include <QLabel>

TFolder* TFileManager::baseFolder=nullptr;
TFolder* TFileManager::deletedFolder=nullptr;
TFolder* TFileManager::searchFolder=nullptr;


TFileManager::TFileManager(ThreadManager* t,mainThread* mt,QVBoxLayout* layout,HoverButton* prev, HoverButton* next, HoverButton* search, TLineEdit *line,TStackedWidget* stackW, QObject *parent)
    : QObject{parent},m_Tmanager(t), m_Mthread(mt), m_prev(prev), m_next(next), m_search(search), m_lineEdit(line), m_stackW(stackW),available(true),m_recursiveSearch(true),m_lastPath("")
{
    searching=false;

    TFileInfo defaultInf;
    defaultInf.filepath=":/";
    baseFolder = new TFolder(defaultInf, true, nullptr,m_Tmanager,m_Mthread ,this,m_stackW);
    deletedFolder = new TFolder(defaultInf, true, nullptr,m_Tmanager,m_Mthread ,this,m_stackW);
    searchFolder = new TFolder(defaultInf, true, nullptr,m_Tmanager,m_Mthread ,this,m_stackW);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, [this] {
        available = true;
    });

    baseFolder->setObjectName("baseTFolder");
    deletedFolder->setObjectName("deletedTFolder");
    searchFolder->setObjectName("searchTFolder");


    baseFolder->setRoundness(15);
    searchFolder->setRoundness(15);
    deletedFolder->setRoundness(15);



    baseFolder->setCornerStyle(TFrame::CornerStyle::Default);
    searchFolder->setCornerStyle(TFrame::CornerStyle::Default);
    deletedFolder->setCornerStyle(TFrame::CornerStyle::Default);

    baseFolder->setFavoriteLayout(layout);
    deletedFolder->setFavoriteLayout(layout);
    searchFolder->setFavoriteLayout(layout);


    baseFolder->setStyleSheet("QFrame{background:transparent;}");
    deletedFolder->setStyleSheet("QFrame{background:transparent;}");
    searchFolder->setStyleSheet("QFrame{background:transparent;}");

    connect(m_prev, &HoverButton::clicked, this, &TFileManager::back);
    connect(m_next, &HoverButton::clicked, this, &TFileManager::next);
    connect(m_search, &HoverButton::clicked, this, &TFileManager::setSearch);

    connect(m_lineEdit, &TLineEdit::searchTextChanged, this, [this](const QString& text) {
        if (!searching) m_search->click();
        searchFile(text);
    });

    connect(m_lineEdit,&TLineEdit::focusEntered,this,[this]{
        if(isSearching()){
            m_search->setIcon(QIcon(":/icons/closeblue.svg"));
        }else{
            m_search->setIcon(QIcon(":/icons/searchblue.svg"));
        }
    });
    connect(m_lineEdit,&TLineEdit::focusLeave,this,[this]{
        if(isSearching()){
            m_search->setIcon(QIcon(":/icons/closewhite.svg"));
        }else{
            m_search->setIcon(QIcon(":/icons/searchwhite.svg"));
        }
    });


    m_stackW->addWidget(baseFolder);
    m_stackW->addWidget(searchFolder);
    m_stackW->addWidget(deletedFolder);
}

void TFileManager::setSearch(bool s)
{
    searching = s;
    if (s) {
        m_search->setIcon(QIcon(":/icons/closeblue.svg"));
        m_stackW->setCurrentWidget(searchFolder);
    } else {
        m_search->setIcon(QIcon(":/icons/searchwhite.svg"));
        m_lineEdit->clear();
        m_lineEdit->clearFocus();
        m_stackW->setCurrentWidget(baseFolder);
    }
}

void TFileManager::searchFile(const QString& str)
{
    if (str.isEmpty()) return;
    if (!searching) setSearch(true);

    TFolder::SFolderRegistry.clear();
    TFolder::SFolderHistory.clear();
    searchFolder->removeAll();

    const QList<eltCore> & tempList = m_recursiveSearch ? TFolder::FolderHistory[TFolder::currentFolderIndex]->getList() : TFolder::FolderHistory[TFolder::currentFolderIndex]->m_eltList; // Avoids detachment

    QList<eltCore> resultList;
    resultList.reserve(tempList.size());

    for (eltCore elt : std::as_const(tempList)) { // Prevent detachment
        if (elt.name.toLower().contains(str.toLower())) {
            resultList.append(elt);
        }
    }

    if(resultList.isEmpty()){
        QLabel* label=new QLabel(searchFolder->fileWidget());
        //label->setTextFormat(Qt::RichText);
        label->setText("🧐 No File Found !");
        label->setStyleSheet("font:700 24pt \"Noto Sans\";");
        label->setMinimumHeight(200);
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        searchFolder->fileWidget()->getLayout()->addWidget(label);
        searchFolder->fileWidget()->getLayout()->setBaseWidth(searchFolder->width()-50);
        return;
    }

    searchFolder->fileWidget()->getLayout()->setBaseWidth(160);

    for (eltCore elt : std::as_const(resultList)) {

        //QString newPath =elt.path.split("/");
        TFileInfo inf=elt.info;
        QStringList list= elt.info.filepath.split("/");
        QString searchPath=":/";
        if(list.size()>=2){
            searchPath+=list.at(list.size()-2)+"/"+list.at(list.size()-1);
        }else{
            searchPath+=list.at(list.size()-1);
        }
        elt.info.filepath=searchPath;
        elt.info.filepath=searchFolder->Organise(elt.info.filepath);
        TFolder::setupSearchFolder(searchFolder,elt.info);

        elt.info=inf;

    }

    searchFolder->fileWidget()->getLayout()->rearrangeWidgets();
}


TFileManager::~TFileManager()
{
    baseFolder->deleteLater();
    deletedFolder->deleteLater();
    searchFolder->deleteLater();
    delete m_timer;
}

void TFileManager::setup(QList<TFileInfo> infoList)
{

    for (TFileInfo &info : infoList) {
        if(info.deleted){
            info.filepath=deletedFolder->Organise(info.filepath);
            TFolder::setupDeletedFolder(deletedFolder,info);
        }else{
            info.filepath=baseFolder->Organise(info.filepath);
            TFolder::setupFolder(baseFolder,info);
        }
    }

    baseFolder->updateInfo();
    deletedFolder->updateInfo();

    TFolder::FolderHistory.append(baseFolder);
    TFolder::currentFolderIndex = 0;

    m_stackW->update();
    m_stackW->setCurrentWidget(baseFolder);

    if(!m_lastPath.isEmpty()) gotoFolder(m_lastPath);

    emit doneSettingUpFileSys();
    deletedFolder->setFavoriteLayout(deletedFolder->favoriteLayout());
    baseFolder->setFavoriteLayout(deletedFolder->favoriteLayout());
    searchFolder->setFavoriteLayout(deletedFolder->favoriteLayout());

}

void TFileManager::setupAsync(QList<TFileInfo> infoList)
{
    if (infoList.isEmpty()) {
        baseFolder->updateInfo();
        deletedFolder->updateInfo();

        TFolder::FolderHistory.append(baseFolder);
        TFolder::currentFolderIndex = 0;

        m_stackW->update();

        if(m_stackW->currentWidget()!=baseFolder){
            m_stackW->setCurrentWidget(baseFolder);
        }else{
            baseFolder->show();
        }

        if(!m_lastPath.isEmpty()) gotoFolder(m_lastPath);

        emit doneSettingUpFileSys();

        deletedFolder->setFavoriteLayout(deletedFolder->favoriteLayout());
        baseFolder->setFavoriteLayout(deletedFolder->favoriteLayout());
        searchFolder->setFavoriteLayout(deletedFolder->favoriteLayout());
        return;
    }

    TFileInfo info = infoList.takeFirst();

    if (info.deleted) {
        info.filepath = deletedFolder->Organise(info.filepath);
        TFolder::setupDeletedFolderAsync(deletedFolder, info);
    } else {
        info.filepath = baseFolder->Organise(info.filepath);
        TFolder::setupFolderAsync(baseFolder, info);
    }

    QTimer::singleShot(10,0, [this, infoList]() mutable {
        this->setupAsync(infoList);
    });
}


void TFileManager::back()
{
    //qDebug() << "current TFolder index"<< TFolder::currentTFolderIndex;
    if (TFolder::currentFolderIndex > 0) {
        if (!available) return;
        available = false;
        m_timer->start(animTime);
        TFolder* previousTFolder = TFolder::FolderHistory[TFolder::currentFolderIndex];
        emit pathChanged(previousTFolder->path());
        TFolder::currentFolderIndex--;
        previousTFolder->setIconView();
    }
}

void TFileManager::next()
{
    if (TFolder::currentFolderIndex < TFolder::FolderHistory.size() - 1) {
        if (!available) return;
        available = false;
        m_timer->start(animTime);
        TFolder::currentFolderIndex++;
        TFolder* nextTFolder = TFolder::FolderHistory[TFolder::currentFolderIndex];
        emit pathChanged(nextTFolder->path());
        nextTFolder->display();
    }
}

void TFileManager::showDeleted()
{
    if (m_stackW->currentWidget() != deletedFolder) {
        m_stackW->setCurrentWidget(deletedFolder);
    }
}

void TFileManager::showHome()
{
    if(isSearching()){m_search->click();}
    if (m_stackW->currentWidget() == baseFolder) {
        m_prev->setDisabled(true);
        while (TFolder::FolderHistory[TFolder::currentFolderIndex] != baseFolder) {
            back();
            TCLOUD::Wait(animTime);
        }
        m_prev->setDisabled(false);
    } else {
        m_stackW->setCurrentWidget(baseFolder);

    }
}


void TFileManager::makeTransparent(bool enable)
{
    baseFolder->setEnableBackground(!enable);
    searchFolder->setEnableBackground(!enable);
    deletedFolder->setEnableBackground(!enable);
}

qint64 TFileManager::usage() const
{
    qint64 usg=baseFolder->getSize()+deletedFolder->getSize();
    return usg;
}

bool TFileManager::recursiveSearch() const
{
    return m_recursiveSearch;
}

void TFileManager::setRecursiveSearch(bool newRecursiveSearch)
{
    m_recursiveSearch = newRecursiveSearch;
}

void TFileManager::refresh()
{
    m_lastPath=TFolder::FolderHistory[TFolder::currentFolderIndex]->path();

    TFolder::reset();

    baseFolder->removeAll();
    searchFolder->removeAll();
    deletedFolder->removeAll();
}

void TFileManager::gotoFolder(const QString &path)
{
    qDebug() << "went to folder :"<< path;
}

QString TFileManager::path() const
{
    return m_path;
}

void TFileManager::setPath(const QString &newPath)
{
    if (m_path == newPath)
        return;
    m_path = newPath;
    emit pathChanged(m_path);
}
