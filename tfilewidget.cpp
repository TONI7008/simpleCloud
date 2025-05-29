#include "tfilewidget.h"
#include "tfolder.h"
#include "tmenu.h"
#include "tfile.h"
#include "threadmanager.h"
#include "mainthread.h"
#include "tgridlayout.h"
#include "tfilechecker.h"
#include "inputdialog.h"
#include "tclipboard.h"

#include <algorithm>
#include <QClipboard>
#include <QApplication>
#include <QActionGroup>
#include <QMimeData>
#include <QMimeDatabase>
#include <QFileInfo>
#include<QMessageBox>
#include <QMimeType>
#include <QFuture>
#include <QMetaObject>
//#include <QtConcurrent>
#include <QThread>


TFileWidget::TFileWidget(TFolder* pfolder,TFileManager* fileM, QWidget* parent)
    : TWidget{parent},m_Pfolder(pfolder),m_fileManager(fileM)
{
    setAcceptDrops(true);
    setWindowFlag(Qt::FramelessWindowHint);

    contextMenu = new TMenu(this);
    selectionMenu = new TMenu(this);

    contextMenu->setBorderRadius(10);
    contextMenu->setBorderSize(4);
    contextMenu->setBorder(true);
    selectionMenu->setBorderRadius(10);
    selectionMenu->setBorder(true);
    selectionMenu->setBorderSize(4);

    m_layout = new TGridLayout(this);
    //m_layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setLayout(m_layout);

    settingUp();

    connect(this, &TFileWidget::resizing, m_layout, &TGridLayout::rearrangeWidgets);
    connect(this, &TFileWidget::previousPressed,this, [this]{
        m_fileManager->back();
    });
    connect(this, &TFileWidget::nextPressed,this, [this]{
        m_fileManager->next();
    });
}

TFileWidget::~TFileWidget() {

    delete contextMenu;
    delete selectionMenu;
    delete m_layout;
    m_fileManager=nullptr;
    fileList.clear();
}

void TFileWidget::add(TCloudElt *elt)
{
    connect(elt, &TCloudElt::Clicked, this, [this, elt]() {
        if (!QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            clearSelection();
            selecting=false;
            selectionRect=QRect();
        }
        elt->setSelected(!elt->isSelected());
        update();
    });
    connect(elt, &TCloudElt::doubleClicked, this, [this]() {
        selecting = false;
        selectionRect = QRect();
        update();
    });


    elt->setFMenu(selectionMenu);
    m_layout->addWidget(elt);
    fileList.append(elt);
}

void TFileWidget::settingUp() {

    QAction* pasteAction = new QAction(QIcon(":/icons/addfolderwhite.svg"), "Paste", contextMenu);
    QAction* createAction = new QAction(QIcon(":/icons/addfolderwhite.svg"), "Create folder", contextMenu);
    QAction* propertyAction = new QAction(QIcon(":/icons/infowhite.svg"), "Property", contextMenu);
    QAction* shareAction = new QAction(QIcon(":/icons/sharefilewhite.svg"), "Share", contextMenu);
    QAction* sizeAction = new QAction(QIcon(":/icons/folder.svg"), "Size", contextMenu);

    // Create a submenu for "Order by"
    TMenu* orderMenu = new TMenu("Sort by",contextMenu);
    QAction* sortbysizeAction = new QAction("Size", orderMenu);
    QAction* sortbynameAction = new QAction("Name", orderMenu);
    QAction* sortbyTypeAction = new QAction("Type", orderMenu);

    TMenu *viewMenu = new TMenu(QIcon(":/icons/gridwhite.svg"),"View mode",contextMenu);
    QAction* lineAction = new QAction(QIcon(":/icons/linewhite.svg"), "Linear display", viewMenu);
    QAction* gridAction = new QAction(QIcon(":/icons/appwhite.svg"), "Grid display", viewMenu);

    viewMenu->addAction(lineAction);
    viewMenu->addAction(gridAction);

    lineAction->setCheckable(true);
    gridAction->setCheckable(true);


    sortbysizeAction->setCheckable(true);
    sortbynameAction->setCheckable(true);
    sortbyTypeAction->setCheckable(true);

    QActionGroup* orderGroup2 = new QActionGroup(this);
    orderGroup2->setExclusive(true);
    orderGroup2->addAction(lineAction);
    orderGroup2->addAction(gridAction);

    QActionGroup* orderGroup = new QActionGroup(this);
    orderGroup->setExclusive(true);
    orderGroup->addAction(sortbynameAction);
    orderGroup->addAction(sortbysizeAction);
    orderGroup->addAction(sortbyTypeAction);

    orderMenu->addAction(sortbynameAction);
    orderMenu->addAction(sortbysizeAction);
    orderMenu->addAction(sortbyTypeAction);

    contextMenu->addAction(createAction);
    contextMenu->addAction(propertyAction);
    contextMenu->addAction(shareAction);
    contextMenu->addAction(pasteAction);
    contextMenu->addAction(sizeAction);
    contextMenu->addSeparator();
    contextMenu->addMenu(orderMenu);
    contextMenu->addSeparator();
    contextMenu->addMenu(viewMenu);

    QAction* copyAction = new QAction(QIcon(":/icons/copywhite.svg"), "Copy", selectionMenu);
    QAction* cutAction = new QAction(QIcon(":/icons/deletewhite.svg"), "Cut", selectionMenu);
    QAction* deleteAction = new QAction(QIcon(":/icons/deletewhite.svg"), "Delete", selectionMenu);
    QAction* downloadAction = new QAction(QIcon(":/icons/copywhite.svg"), "Download", selectionMenu);

    m_sizeAction = new QAction("size",selectionMenu);
    m_sizeAction->setEnabled(false);

    selectionMenu->addAction(m_sizeAction);
    selectionMenu->addSeparator();
    selectionMenu->addAction(copyAction);
    selectionMenu->addAction(cutAction);
    selectionMenu->addAction(deleteAction);
    selectionMenu->addAction(downloadAction);


    connect(this, &TWidget::rightClicked, this, [this](const QPoint& pos) {
        if (hasSelectedItems()) {
            updateSelectionMenu();
            selectionMenu->exec(mapToGlobal(pos));
        } else {
            contextMenu->exec(mapToGlobal(pos));
        }
    });

    connect(this, &TWidget::Clicked, this, [this](const QPoint& pos) {
        selecting = true;
        selectionStart = pos;  // Use the exact position from the Clicked signal
        selectionRect = QRect(selectionStart, QSize());

        if (!QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            clearSelection();  // Clear selection if Ctrl is not held
        }
    });

    connect(createAction, &QAction::triggered, this, [this] {
       InputDialog* ins=InputDialog::instance();
        ins->setPurpose(InputDialog::CreateFolder);
        *ins << getPath();
        ins->show();

        QEventLoop loop;
        connect(ins, &InputDialog::DoneCreatingFolder,&loop,[this,&loop](const QString& str){
            createDir(str+"/.");
            loop.quit();
        });
        connect(ins, &InputDialog::canceling,&loop,&QEventLoop::quit);
        loop.exec();

    });

    connect(lineAction, &QAction::triggered, this, [&] {
        if (m_path == ":") {
            setView(TCLOUD::View::Linear);
        }
    });

    connect(gridAction, &QAction::triggered, this, [&] {
        setView(TCLOUD::View::Grid);
    });

    connect(sortbysizeAction, &QAction::triggered, this, [&](bool ascendant) {
        QList<TCloudElt*> tempList = fileList;
        sort(tempList, ascendant);

        // Convert QList<TCloudElt*> to QList<QWidget*>
        QList<QWidget*> widgetList;
        for (TCloudElt* elt : std::as_const(tempList)) {
            if (elt) widgetList.append(static_cast<QWidget*>(elt)); // Ensure it's a QWidget or derived
        }

        m_layout->addWidgetList(widgetList);
        m_layout->rearrangeWidgets();
    });

    connect(sortbynameAction, &QAction::triggered, this, [&](bool order){
        QList<TCloudElt*> tempList = fileList;
        sortName(tempList,order);

        // Convert QList<TCloudElt*> to QList<QWidget*>
        QList<QWidget*> widgetList;
        for (TCloudElt* elt : std::as_const(tempList)) {
            if (elt) widgetList.append(static_cast<QWidget*>(elt)); // Ensure it's a QWidget or derived
        }

        m_layout->addWidgetList(widgetList);
        m_layout->rearrangeWidgets();
    });
    connect(sortbyTypeAction, &QAction::triggered, this, [&](bool order){
        QList<TCloudElt*> tempList = fileList;
        sortType(tempList,order);

        // Convert QList<TCloudElt*> to QList<QWidget*>
        QList<QWidget*> widgetList;
        for (TCloudElt* elt : std::as_const(tempList)) {
            if (elt) widgetList.append(static_cast<QWidget*>(elt)); // Ensure it's a QWidget or derived
        }

        m_layout->addWidgetList(widgetList);
        m_layout->rearrangeWidgets();
    });


    connect(pasteAction,&QAction::triggered,this,&TFileWidget::handlePaste);

    connect(copyAction, &QAction::triggered, this, &TFileWidget::copySelectedItems);
    connect(deleteAction, &QAction::triggered, this, &TFileWidget::deleteSelectedItems);
    connect(cutAction, &QAction::triggered, this, &TFileWidget::cutSelectedItems);
    connect(downloadAction, &QAction::triggered, this, &TFileWidget::downloadSelectedItems);

    setMinimumWidth(250);
    QEasingCurve easing(QEasingCurve::Linear);
    easing.setAmplitude(0);
    easing.setPeriod(1.0);

    m_layout->setVerticalSpacing(15);
    m_layout->setHorizontalSpacing(15);
    m_layout->setContentsMargins(2,2,2,2);

    Qt::Alignment align=Qt::AlignTop | Qt::AlignHCenter;
    m_layout->setAlignment(align);
    m_layout->setEasingCurve(easing);

    sortbynameAction->setChecked(true);
    gridAction->setChecked(true);
}

// Helper function to process the `FileList`
void TFileWidget::processListSelection(const std::function<void(TCloudElt*)>& process) {
    if (fileList.size() > 20) {
        QtConcurrent::blockingMap(fileList, process);
    } else {
        for (auto* elt : std::as_const(fileList)) {
            process(elt);
        }
    }
}

void TFileWidget::updateSelectionMenu()
{
    // Calculate total size (using your existing logic)
    m_sizeAction->setEnabled(true);
    std::atomic<quint64> totalSize(0);
    auto accumulateSize = [&totalSize](auto* frame) {
        if (frame->isSelected()) {
            totalSize += frame->size();
        }
    };

    if (fileList.size() > 10) {
        QtConcurrent::blockingMap(fileList, accumulateSize);
    } else {
        for (auto* frame : std::as_const(fileList)) {
            accumulateSize(frame);
        }
    }

    // Update the persistent size action
    QString sizeText = TCLOUD::formatSize(totalSize.load());
    m_sizeAction->setText(sizeText);
    selectionMenu->setTitle(sizeText);
}


bool TFileWidget::hasSelectedItems() const {
    // Early exit for empty lists
    if (fileList.empty()) return false;

    // Sequential check is faster for small lists
    if (fileList.size() <= 20) {
        for (auto* frame : fileList) {
            if (frame->isSelected()) return true;
        }
        return false;
    }

    // Parallel check for large lists
    std::atomic<bool> selected(false);
    QtConcurrent::blockingFiltered(fileList, [&selected](auto* frame) {
        if (!selected.load() && frame->isSelected()) {
            selected.store(true);
        }
        return false; // We don't actually want to filter, just check
    });
    return selected.load();
}

void TFileWidget::updateSelectionRectangle(const QPoint& endPoint) {
    selectionRect = QRect(selectionStart, endPoint).normalized();
}

void TFileWidget::selectItemsInRectangle() {
    // Throttle selection updates during mouse movement
    static QElapsedTimer throttleTimer;
    if (throttleTimer.isValid() && throttleTimer.elapsed() < 16) { // ~60fps
        return;
    }
    throttleTimer.start();

    // Only update visible items for better performance
    QRect visibleRect = rect();
    auto processSelection = [this, visibleRect](auto* elt) {
        if (visibleRect.intersects(elt->geometry())) {
            bool isSelected = selectionRect.intersects(elt->geometry());
            elt->setSelected(isSelected);
            elt->setMultiSelection(isSelected);
        }
    };

    processListSelection(processSelection);
    updateSelectionMenu();
    update(); // Request a single repaint
}

void TFileWidget::handlePaste()
{
    TClipBoard* clipBoard=TClipBoard::instance();
    //TClipBoard::transactionType type=clipBoard->type();

    for(auto &elt : clipBoard->items()){
        if(!TFileChecker::instance()->isOk(elt.name,m_Pfolder)){
            qDebug() << elt.name <<" already exists !";
            continue;
        }
        //qDebug() <<"element names ="<< elt.name ;

        elt.info.filepath=getPath()+"/"+elt.name;

        elt.info.filepath=TFileManager::baseFolder->Organise(elt.info.filepath);
        TFolder::setupFolder(TFileManager::baseFolder,elt.info);

        m_Pfolder->updateInfo();
    }
    m_Pfolder->setFavoriteLayout(m_Pfolder->m_favoriteLayout);

    clipBoard->clear();
}

void TFileWidget::clearSelection() {
    // Only clear items that are actually selected
    auto processClearSelection = [](auto* frame) {
        if (frame->isSelected()) {
            frame->setSelected(false);
            frame->setMultiSelection(false);
        }
    };

    processListSelection(processClearSelection);
}


void TFileWidget::mouseMoveEvent(QMouseEvent* event) {
    if (selecting) {
        updateSelectionRectangle(event->pos());
        selectItemsInRectangle();
    }
    TWidget::mouseMoveEvent(event);
}

void TFileWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    if (selecting) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, false); // Faster without antialiasing
        painter.setPen(QPen(QColor(71,158,245), 2));
        painter.setBrush(QColor(8, 78, 115, 153));
        painter.drawRect(selectionRect);
    }
}

void TFileWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && selecting) {
        selecting = false;
        selectionRect = QRect();
        update();
    }
    QWidget::mouseReleaseEvent(event);
}


void TFileWidget::copySelectedItems() {
    auto processSelection = [](auto* elt) {
        if (elt && elt->isSelected()) {
            elt->copy();
        }
    };

    processListSelection(processSelection);
}

void TFileWidget::cutSelectedItems()
{
    auto processSelection = [](auto* elt) {
        if (elt && elt->isSelected()) {
            elt->cut();
        }
    };

    processListSelection(processSelection);
}

void TFileWidget::downloadSelectedItems()
{
    auto processSelection = [](auto* elt) {
        if (elt->isSelected() && elt->type()==TCloudElt::File) {
            TFile* file=qobject_cast<TFile*>(elt);
            file->downloadFile();
        }
    };

    processListSelection(processSelection);
}

void TFileWidget::setFileManager(TFileManager * fManager)
{
    m_fileManager=fManager;
}

void TFileWidget::deleteSelectedItems() {
    // Implement delete logic here
    qDebug() << "Deleting selected items...";
}


void TFileWidget::setPath(QString path) {
    m_path = path;
}

QString TFileWidget::getPath() {return m_Pfolder->path();}


void TFileWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void TFileWidget::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void TFileWidget::dropEvent(QDropEvent *event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl& url : std::as_const(urlList)) {
            QString fileName = url.toLocalFile();
            QFileInfo inf(fileName);
            if(TFileChecker::instance()->isOk(inf.fileName(),m_Pfolder)){
                if (!fileName.isEmpty() && m_Pfolder->m_mThread) {
                    TFile* frame=makeTempFrame(fileName);
                    m_Pfolder->m_Tmanager->upload(fileName,m_Pfolder->m_mThread->getUsername(),frame->loader());
                    QMetaObject::Connection conn;
                    conn = connect(m_Pfolder->m_Tmanager, &ThreadManager::uploadDone, this, [=](TFileInfo inf) {
                        frame->setInfo(inf);
                        // Safely disconnect after upload completion
                        disconnect(m_Pfolder->m_Tmanager, &ThreadManager::uploadDone, this, nullptr);
                    });
                }
            }else{
                QMessageBox::critical(this,"identicals files","un fichier contenant le meme nom existe deja veuillez le renomer");
            }

        }
    }
    event->acceptProposedAction();
}
TFile* TFileWidget::makeTempFrame(QString filename){
    TFileInfo info;
    info.mimetype=getMimeType(filename);
    info.filepath=m_path+"/"+filename.split("/").last();
    TFile* frame= new TFile(m_Pfolder,info,this);
    m_Pfolder->add(frame);
    return frame;
}

void TFileWidget::sort(QList<TCloudElt*>& liste, bool ordreCroissant) {
    auto compare = [ordreCroissant](const TCloudElt* a, const TCloudElt* b) {
        qint64 sizeA = a->size(), sizeB = b->size();
        return ordreCroissant ? sizeA < sizeB : sizeA > sizeB;
    };

    std::sort(liste.begin(), liste.end(), compare);
}

void TFileWidget::sortName(QList<TCloudElt*>& liste, bool ordreCroissant) {
    auto compare = [ordreCroissant](const TCloudElt* a, const TCloudElt* b) {
        QString nameA = a->name().toLower();  // Convert to lowercase for case-insensitive comparison
        QString nameB = b->name().toLower();
        return ordreCroissant ? nameA < nameB : nameA > nameB;
    };

    std::sort(liste.begin(), liste.end(), compare);
}

void TFileWidget::sortType(QList<TCloudElt*>& liste, bool foldersFirst) {
    auto compare = [foldersFirst](const TCloudElt* a, const TCloudElt* b) {
        // First compare types
        bool aIsFolder = (a->type() == TCloudElt::Folder);
        bool bIsFolder = (b->type() == TCloudElt::Folder);

        if (aIsFolder != bIsFolder) {
            return foldersFirst ? aIsFolder : bIsFolder;
        }

        // If same type, compare names (case-insensitive)
        QString nameA = a->name().toLower();
        QString nameB = b->name().toLower();
        return nameA < nameB;
    };

    std::sort(liste.begin(), liste.end(), compare);
}



void TFileWidget::remove(TCloudElt *elt)
{
    fileList.removeOne(elt);
    m_layout->removeWidgetAnimated(elt);
}
void TFileWidget::softRemove(TCloudElt *elt)
{
    m_layout->removeWidget(elt);
    fileList.removeOne(elt);
}

void TFileWidget::Clear() {
    fileList.clear();
    m_layout->removeAll();
}

void TFileWidget::createDir(QString name)
{
    TFileInfo inf;
    inf.filepath=name;

    TFolder* folder = new TFolder(inf,false,m_Pfolder,m_Pfolder->m_Tmanager,m_Pfolder->m_mThread,m_fileManager,this);
    m_Pfolder->add(folder);
    fileList.append(folder);
    m_Pfolder->updateInfo();
}

void TFileWidget::setView(TCLOUD::View view) {
    if (view == TCLOUD::View::Linear) {
        setLinearView();
    } else {
        setGridView();
    }
}

void TFileWidget::setLinearView() {
    for (auto &f : fileList) {
        Q_UNUSED(f);
        //f->setView(TCLOUD::View::Linear);
    }
    //m_layout->setView(TCLOUD::View::Linear);
}

void TFileWidget::setGridView() {
    for (auto &f : fileList) {
        Q_UNUSED(f);
        //f->setView(TCLOUD::View::Grid);
    }
   // m_layout->setView(TCLOUD::View::Grid);
}
QString TFileWidget::getMimeType(const QString &filePath) {
    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

    if (mimeType.isValid()) {
        return mimeType.name(); // Return the MIME type as a QString
    } else {
        return "application/octet-stream"; // Default MIME type if unknown
    }
}
