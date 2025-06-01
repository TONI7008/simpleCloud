#ifndef TTFileWidget_H
#define TTFileWidget_H

#include "tfilemanager.h"
#include "twidget.h"
#include "tcloud.h"
#include <QtConcurrent/QtConcurrent>

class TCloudElt;
class TMenu;
class TFolder;
class TFile;
class mainThread;
class ThreadManager;
class TGridLayout;


class TFileWidget : public TWidget
{
    Q_OBJECT

public:
    explicit TFileWidget(TFolder*,TFileManager* fileM,QWidget *parent = nullptr);
    ~TFileWidget();

    void add(TCloudElt*);
    void remove(TCloudElt*);
    void softRemove(TCloudElt *elt);
    void Clear();
    void setView(TCLOUD::View);

    void sortSize(bool order=true);
    void sortName(bool order=true);
    void sortType(bool order=true);

    TGridLayout* getLayout(){
        return m_layout;
    }


    void clearSelection();
    void setPath(QString path);
    QString getPath();

    void deleteSelectedItems();
    void copySelectedItems();
    void cutSelectedItems();
    void downloadSelectedItems();

    TMenu* getSelectionMenu(){
        return selectionMenu;
    }
    TFile *makeTempFrame(QString filename);
    void setFileManager(TFileManager*);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void processListSelection(const std::function<void (TCloudElt*)> &process);

    void createDir(QString);
    void setLinearView();
    void setGridView();
    void updateSelectionRectangle(const QPoint& endPoint);
    void selectItemsInRectangle();

    QString m_path;
    TFolder* m_Pfolder=nullptr;
    TMenu *contextMenu=nullptr;
    TMenu* selectionMenu=nullptr;
    TGridLayout* m_layout=nullptr;
    TFileManager* m_fileManager=nullptr;
    QList<TCloudElt*> fileList;
    bool linearview=false;

    bool selecting = false;
    QPoint selectionStart;
    QRect selectionRect;


    void handlePaste();

    bool hasSelectedItems() const;
    void updateSelectionMenu();
    void settingUp();

    QAction* m_sizeAction=nullptr;

    QString getMimeType(const QString &filePath);

    void sortSize(QList<TCloudElt*> &liste, bool ordreCroissant);
    void sortName(QList<TCloudElt *> &liste, bool ordreCroissant);
    void sortType(QList<TCloudElt *> &liste, bool foldersFirst);
};

#endif // TTFileWidget_H
