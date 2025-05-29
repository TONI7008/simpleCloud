#ifndef TTFile_H
#define TTFile_H

#include "tcloudelt.h"
#include "tfileinfo.h"
#include <QVBoxLayout>

class TMenu;
class TFolder;
class mainThread;
class ThreadManager;
class Loader;
class InfoPage;

class TFile : public TCloudElt {
    Q_OBJECT
public:
    TFile(TFolder* folder,TFileInfo info,QWidget* parent=nullptr);
    ~TFile();
    QString filename(){
        return m_name;
    }
    QString filePath();
    void setStyle(bool);

    TFile* _copy();
    TFolder* parentFolder();
    void setParentFolder(TFolder*);
    qint64 getSize(){return m_size;}
    void setInfo(TFileInfo);
    Loader* loader() const;
    TFileInfo info(){return m_info;}

    QWidget* originParent() override;
    void copy() override;
    void cut() override;

private:
    TFolder* m_pFolder=nullptr;
    bool downloaded=false;
    bool isDownloading=false;
    bool canOpen=false;

    TMenu* contextMenu=nullptr;

    Loader* m_load=nullptr;
    TFrame* m_tFrame=nullptr;

    QString extractFilename(QString path);

    friend class TFolder;
    friend class TFileWidget;

    QIcon getIcon();
    void settingUpSignals();
    void settingUpMenu();
    void open();

    void renamFile();
    void deleteFile();
    void moveToTrash();
    void downloadFile();
    void openFile();

    eltCore core() override;
};

inline QString initialStyle=R"(QFrame{border:none;background:transparent;border-radius:15px;}
                    QPushButton{border:none;background:transparent;/*rgba(228, 71, 66, 0.8);*/}
                    QPushButton:pressed{border:none;background:transparent;}
                    QWidget{font:11pt \"Noto Sans\";}
                    QFrame:hover{
                                    border:1px solid rgb(71,158,245);
                                    background:rgba(189,189,189, 0.3);border-radius:15px;
                                }#m_labelName{background:transparent;border:none;})";
inline QString selectedStyle=R"(QFrame{
                                    border:1px solid rgb(71,158,245);
                                    background:rgba(189,189,189, 0.3);border-radius:15px;}
                                    QPushButton:pressed{border:none;background:transparent;}
                                    #m_labelName{background:transparent;border:none;})";

#endif // TTFile_H
