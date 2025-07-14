#include "tfilemanager.h"
#include "tfolder.h"
#include "hoverbutton.h"
#include "tgridlayout.h"
#include "tfile.h"
#include "tfilewidget.h"
#include "tstackedwidget.h"
#include "iconcolorizer.h"
#include "tlineedit.hpp"
#include "waitingspinner.h"

#include <QLabel>

TFolder* TFileManager::baseFolder=nullptr;
TFolder* TFileManager::deletedFolder=nullptr;
TFolder* TFileManager::searchFolder=nullptr;


TFileManager::TFileManager(ThreadManager* t,NetworkAgent* mt,QVBoxLayout* layout,
                           HoverButton* prev, HoverButton* next, HoverButton* search,
                           TLineEdit *line,TStackedWidget* stackW,
                           QObject *parent)
    : QObject{parent},m_Tmanager(t),
    m_Mthread(mt),
    m_prev(prev),
    m_next(next),
    m_search(search),
    m_lineEdit(line),
    m_stackW(stackW),
    available(true),
    m_recursiveSearch(true),
    m_lastPath("")
{
    searching=false;

    TFileInfo defaultInf;
    defaultInf.filepath=":/";

    TFolder::setDisplayStackWidget(m_stackW);

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


    baseFolder->setBorderRadius(15);
    searchFolder->setBorderRadius(15);
    deletedFolder->setBorderRadius(15);



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
        m_spinner->start();
        helperFolder=TFolder::currentFolder();
        if(isSearching()){
            m_search->setIcon(IconColorizer::colorize(QIcon(":/icons/closewhite.svg")));
        }else{
            m_search->setIcon(IconColorizer::colorize(QIcon(":/icons/searchwhite.svg")));
        }
    });
    connect(m_lineEdit,&TLineEdit::focusLeave,this,[this]{
        if(isSearching()){
            m_search->setIcon(QIcon(":/icons/closewhite.svg"));
        }else{
            m_search->setIcon(QIcon(":/icons/searchwhite.svg"));
        }
    });


    searchFolder->openFolder();
    deletedFolder->openFolder();
    baseFolder->openFolder();

    m_spinner=new WaitingSpinnerWidget(searchFolder->m_scrollArea,true,true);
    m_spinner->setMinimumTrailOpacity(20.0);
    m_spinner->setTrailFadePercentage(50.0);
    m_spinner->setNumberOfLines(12);
    m_spinner->setLineLength(16);
    m_spinner->setLineWidth(5);
    m_spinner->setInnerRadius(14);
    m_spinner->setRevolutionsPerSecond(1.5);
    m_spinner->setColor(IconColorizer::dominantColor());
    m_spinner->hide();
}

void TFileManager::setSearch(bool s)
{
    searching = s;
    if (s) {
        m_search->setIcon(IconColorizer::colorize(QIcon(":/icons/closewhite.svg")));
        searchFolder->openFolder();

    } else {
        m_search->setIcon(QIcon(":/icons/searchwhite.svg"));
        m_lineEdit->clear();
        m_lineEdit->clearFocus();
        baseFolder->openFolder();

    }
}

void TFileManager::searchFile(const QString& str)
{
    m_spinner->start();

    if (str.isEmpty()) return;
    if (!searching) setSearch(true);

    TFolder::SFolderRegistry.clear();
    searchFolder->removeAll();

    TFolder* folder=helperFolder;
    const QList<eltCore> & tempList = m_recursiveSearch ? folder->getList() : folder->m_eltList; // Avoids detachment

    QList<eltCore> resultList;
    resultList.reserve(tempList.size());

    qDebug() << "next list size :"<<nextList.size();

    for (eltCore elt : std::as_const(tempList)) {        // Prevent detachment
        //if(elt)
        if (elt.name.toLower().contains(str.toLower()) && elt.type==TCloudElt::File) {
            resultList.append(elt);
        }
    }


    if(resultList.isEmpty()){
        m_spinner->stop();
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
    m_spinner->stop();
}



TFileManager::~TFileManager()
{
    baseFolder->deleteLater();
    deletedFolder->deleteLater();
    searchFolder->deleteLater();
    delete m_timer;
    if(m_spinner){
        delete m_spinner;
    }
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

    baseFolder->openFolder();

    m_stackW->update();
    TCLOUD::Wait(100);

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

        baseFolder->openFolder();

        m_stackW->update();

        if(!m_lastPath.isEmpty()) gotoFolder(m_lastPath);


        deletedFolder->setFavoriteLayout(deletedFolder->favoriteLayout());
        baseFolder->setFavoriteLayout(deletedFolder->favoriteLayout());
        searchFolder->setFavoriteLayout(deletedFolder->favoriteLayout());

        //qDebug() << "tfolder folder registry start";
        //qDebug() << TFolder::FolderRegistry;
        //qDebug() << "tfolder folder registry stop";

        TCLOUD::Wait(100);
        emit doneSettingUpFileSys();
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
    nextList.append(TFolder::currentFolder());
    TFolder::currentFolder()->openPreviousFolder();
    emit pathChanged(TFolder::currentFolder()->path());
}

void TFileManager::next()
{
    if(!nextList.isEmpty()){
        TFolder* nFolder=nextList.takeLast();

        nFolder->openFolder();
        emit pathChanged(nFolder->path());
    }
}

void TFileManager::showDeleted()
{
    deletedFolder->openFolder();
    emit pathChanged(deletedFolder->path());
}

void TFileManager::showHome()
{
    if(isSearching()){m_search->click();}
    baseFolder->openFolder();
    emit pathChanged(baseFolder->path());
}


void TFileManager::makeTransparent(bool enable)
{
    if(!enable){
        m_stackW->setStyleSheet("background:rgba(40,40,40,0.7);");

    }else{
        m_stackW->setStyleSheet("");
    }
    m_stackW->update();

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
    m_lastPath=TFolder::currentFolder()->path();

    TFolder::reset();

    baseFolder->removeAll();
    searchFolder->removeAll();
    deletedFolder->removeAll();
}

void TFileManager::gotoFolder(const QString &path)
{
    TFolder* folder=TFolder::FolderRegistry[path];
    if(folder){
        folder->openFolder();
    }else{
        qDebug() << "failed to go to "<<path;
    }
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
