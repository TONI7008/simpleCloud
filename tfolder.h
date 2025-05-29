#ifndef TFolder_H
#define TFolder_H

#include <QWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QMap>
#include "tcloudelt.h"
#include "tfileinfo.h"


class QAnimatedGridLayout;
class TFile;
class TWidget;
class TFileManager;
class mainThread;
class ThreadManager;
class InfoPage;
class TFileWidget;
class TMenu;


class TFolder : public TCloudElt
{
    Q_OBJECT
public:
    explicit TFolder(TFileInfo info, bool base=false,TFolder* pTFolder=nullptr,ThreadManager* t=nullptr,mainThread* mt=nullptr,TFileManager* fm=nullptr,QWidget *parent = nullptr);
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
    static QList<TFolder*> FolderHistory;

    static QMap<QString, TFolder*> SFolderRegistry;
    static QList<TFolder*> SFolderHistory;

    static QMap<QString, TFolder*> DFolderRegistry;
    static QList<TFolder*> DFolderHistory;
    static int currentFolderIndex;

    void setStyle(bool);
    void setZoomed(bool zoom);

    bool isZoomed(){return m_isZoomed;}
    TFolder *_copy();
    void copyFolder(QString old="");

    void cutFolder();
    qint64 getSize();
    mainThread* getMainThread() const;
    ThreadManager* getThreadManager() const;
    QWidget* originParent() override;
    QVBoxLayout *favoriteLayout() const;
    void setFavoriteLayout(QVBoxLayout *newFavoriteLayout);

signals:
    void Zoom(TFolder* wid);

private:

    void handleRightClicked(const QPoint&);

    mainThread* m_mThread;
    ThreadManager* m_Tmanager;
    TFileManager* m_fManager=nullptr;
    TFolder* m_pTFolder=nullptr;

    bool m_base;
    bool m_isZoomed=false;
    bool downloaded=false;
    bool hasBookmark=false;
    QMap<QString, TFolder*> subFolders;
    QList<eltCore> m_eltList;

    QList<TCloudElt*> m_WfileList;

    TFileWidget*m_fileWidget=nullptr;
    TWidget* folderIconView=nullptr;
    TMenu* contextMenu=nullptr;
    QVBoxLayout* m_favoriteLayout=nullptr;

    QScrollArea* m_scrollArea=nullptr;

    QString extractFolderName(QString path);
    void setIconView();
    void setFileView();
    void updateInfo();

    eltCore core() override;
    void copy() override;
    void cut() override;

    void handleRename();
    void moveToTrash();
    void deleteFolder();
    void restoreFolder();

    void toggleZoom();
    void setupFileView();
    void setupFolderIconView();
    void setupScrollArea();
    TFolder* zoomedElt();

    void display(bool d=true);


    friend class TFileManager;
    friend class TFileWidget;
    friend class TFileChecker;

    void settingUpMenu();
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
