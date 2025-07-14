#ifndef TFolder_H
#define TFolder_H

#include <QWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QMap>
#include "tcloudelt.h"
#include "tfileinfo.hpp"


class QAnimatedGridLayout;
class TFile;
class TWidget;
class TFileManager;
class NetworkAgent;
class ThreadManager;
class InfoPage;
class TFileWidget;
class TMenu;
class TStackedWidget;


class TFolder : public TCloudElt
{
    Q_OBJECT
public:
    enum SortType{
        Name,
        Size,
        Type
    };

    explicit TFolder(TFileInfo info, bool base=false,TFolder* pTFolder=nullptr,ThreadManager* t=nullptr,NetworkAgent* mt=nullptr,
                     TFileManager* fm=nullptr,QWidget *parent = nullptr);
    ~TFolder();

    void add(TCloudElt* elt);
    void remove(TCloudElt* file);
    void softRemove(TCloudElt *elt);
    void removeAll();
    void rename(QString);

    QString path() const;

    QList<eltCore> getList() const;

    TFolder* parentFolder();
    void setParentFolder(TFolder*);
    QString Organise(QString path);

    TFileWidget* fileWidget();

    static bool setupFolder(TFolder* pFolder,TFileInfo);
    static bool setupSearchFolder(TFolder *pFolder, TFileInfo finalInfo);
    static bool setupDeletedFolder(TFolder *pFolder, TFileInfo finalInfo);
    static void setupFolderAsync(TFolder *pFolder, TFileInfo finalInfo);
    static void setupDeletedFolderAsync(TFolder *pFolder, TFileInfo finalInfo);

    static void reset();


    static QMap<QString, TFolder*> FolderRegistry;

    static QMap<QString, TFolder*> SFolderRegistry;

    static QMap<QString, TFolder*> DFolderRegistry;

    static int currentFolderIndex;

    static void setDisplayStackWidget(TStackedWidget*);

    void setStyle(bool);

    bool isZoomed(){return m_isZoomed;}
    TFolder *_copy();
    void copyFolder(QString old="");
    void cutFolder(QString old="");

    //void copyFolder(QString path,QString old="");
    //void cutFolder(QString path);

    qint64 getSize();
    NetworkAgent* getNetworkAgent() const;
    ThreadManager* getThreadManager() const;
    QWidget* originParent() override;
    QVBoxLayout *favoriteLayout() const;
    void setFavoriteLayout(QVBoxLayout *newFavoriteLayout);

    SortType sortType() const;
    void setSortType(SortType newSortType,bool order=true);


    static TFolder *currentFolder();

private:

    void handleRightClicked(const QPoint&);

    NetworkAgent* m_mThread;
    ThreadManager* m_Tmanager=nullptr;
    TFileManager* m_fManager=nullptr;
    TFolder* m_pTFolder=nullptr;
    QScrollArea *m_scrollArea=nullptr;

    static TStackedWidget* displayStackWidget;
    static TFolder* m_currentFolder;


    bool m_base;
    bool m_isZoomed=false;
    bool downloaded=false;
    bool hasBookmark=false;
    SortType m_sortType=Size;

    QMap<QString, TFolder*> subFolders;
    QList<eltCore> m_eltList;

    QList<TCloudElt*> m_WfileList;

    TFileWidget*m_fileWidget=nullptr;
    TMenu* contextMenu=nullptr;
    QVBoxLayout* m_favoriteLayout=nullptr;


    QString extractFolderName(QString path);

    void updateInfo();

    eltCore core() override;
    void copy() override;
    void cut() override;
    void deleteElt() override;

    void handleRename();
    void moveToTrash();
    void deleteFolder();
    void restoreFolder();

    void toggleZoom();
    //void setupFileView();
    void setupFolderIconView();
    void setupScrollArea();


    friend class TFileManager;
    friend class TFileWidget;
    friend class TFileChecker;
    friend class TClipBoard;

    void settingUpMenu();

    void openFolder();
    void openPreviousFolder();
    void collectClipboardItems(QList<eltCore> &out, const QString &base);
};


class TFolderLink : public TCloudElt {
    Q_OBJECT
public:
    TFolderLink(TFileInfo info,QVBoxLayout* layout,TFileManager* fm=nullptr,QWidget* parent=nullptr);
    ~TFolderLink();
signals:
    void aboutToBeRemoved();
private:
    TFileManager* m_Manager=nullptr;
    QVBoxLayout* m_layout=nullptr;

    QString extractName(const QString&);
    void connectingSignals();
    void gotoFolder();
    short pos=4;
};

inline QString initialStyleSheet = R"(QFrame{background:transparent;border:none;}
    QPushButton {background: transparent;border: none;}
    QPushButton:pressed{border:none;background:transparent;}
    #m_labelName {background: transparent;color: white;border: none;font-size: 14px;}
#m_scrollArea{background:transparent;border:none;}#folderIconView{background:transparent;border:none;}
QFrame:hover {background:rgba(189,189,189,0.4);border-radius: 15px;border:1px solid rgb(71,158,245);})";

inline QString selectedStyleSheet = R"(QFrame {background:rgba(189,189,189,0.4);border-radius: 15px;border:1px solid rgb(71,158,245);}
QPushButton {background: transparent;border: none;}
QPushButton:pressed {background: transparent;border: none;}
#m_labelName {background: transparent;color: white;border: none;font-size: 14px;}
#m_scrollArea{background:transparent;border:none;}#folderIconView{background:transparent;border:none;}#m_fileWidget{background:transparent;border:none;})";

#endif // TFolder_H
