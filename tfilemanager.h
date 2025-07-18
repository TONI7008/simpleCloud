#ifndef TFileManager_H
#define TFileManager_H

#include <QObject>
#include <QLineEdit>
#include <QStackedWidget>
#include <QVBoxLayout>




class TFolder;
class TWidget;
class HoverButton;
class TStackedWidget;
class ThreadManager;
class NetworkAgent;
class TFileInfo;
class TLineEdit;
class dynamicFrameAssistant;
class WaitingSpinnerWidget;



class TFileManager : public QObject
{
    Q_OBJECT
public:
    explicit TFileManager(ThreadManager*,NetworkAgent*,QVBoxLayout*,
                          HoverButton* prev, HoverButton* next,HoverButton* search,
                          TLineEdit *line,TStackedWidget* stackW,QObject *parent = nullptr);
    ~TFileManager();
    static TFolder* baseFolder;
    static TFolder* deletedFolder;
    static TFolder* searchFolder;


    void setup(QList<TFileInfo>);
    void setupAsync(QList<TFileInfo>);
    void back();
    void next();
    void showDeleted();
    void showHome();
    void setSearch(bool);
    void searchFile(const QString&);
    void makeTransparent(bool);
    bool isSearching(){return searching;}
    qint64 usage() const;

    bool recursiveSearch() const;
    void setRecursiveSearch(bool newRecursiveSearch);
    void refresh();

    void gotoFolder(const QString&);

    QString path() const;
    void setPath(const QString &newPath);

signals:
    void doneSettingUpFileSys();
    void pathChanged(const QString&);

private:
    QTimer* m_timer=nullptr;
    short animTime=305;


    ThreadManager* m_Tmanager=nullptr;
    NetworkAgent* m_Mthread=nullptr;

    HoverButton* m_prev=nullptr;
    HoverButton* m_next=nullptr;
    HoverButton* m_search=nullptr;
    TLineEdit *m_lineEdit=nullptr;
    TStackedWidget* m_stackW=nullptr;
    WaitingSpinnerWidget* m_spinner=nullptr;

    bool searching;
    bool available;
    bool m_recursiveSearch;

    QString m_lastPath;
    QString m_path;

    QList<TFolder*> nextList;
    TFolder* helperFolder=nullptr;

};

#endif // TFileManager_H
