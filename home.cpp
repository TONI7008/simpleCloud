#include "home.h"
#include "ui_home.h"
#include "infopage.h"
#include "inputdialog.h"

#include "tfilemanager.h"
#include "loadingframe.h"
#include "networkagent.h"
#include "tsecuritymanager.h"
#include "setting.h"
#include "settingmanager.h"
#include "threadmanager.h"
#include "tcloud.hpp"
#include "tfilechecker.h"
#include "tclipboard.h"
#include "loginpage.h"
#include "tnotification.h"
#include "dynamicframeassistant.h"
#include "signuppage.h"
#include "tfolder.h"
#include "tflowlayout.h"
#include "iconcolorizer.h"
#include "barupdater.hpp"


Home::Home(TSecurityManager* smanager,QWidget *parent)
    : TWidget(parent)
    ,ui(new Ui::Home)
{
    ui->setupUi(this);
    m_layout1=new TFlowLayout(ui->homePage);
    //m_layout1->setAnimationDuration(1000);
    ui->homePage->setLayout(m_layout1);

    m_layout1->setVerticalSpacing(5);
    m_layout1->setHorizontalSpacing(5);

    m_layout1->addWidget(ui->optionFrame, 0, 0, 2, 1);
    m_layout1->addWidget(ui->pFrame, 0, 1, 1, 1);
    m_layout1->addWidget(ui->dynamicFrame, 1, 1, 1, 1);
    m_layout1->setContentsMargins(5,5,5,5);

    m_layout1->invalidate();  // marks dirty = true
    m_layout1->update();

    QLinearGradient gradient(0, 0, 1, 0);
    gradient.setColorAt(0, QColor("#007aff")); // stop:0 #007aff
    gradient.setColorAt(1, QColor("#00cfff")); // stop:1 #00cfff
    ui->customProgressbar->setChunkGradient(gradient);
    ui->customProgressbar->setBackgroundColor(Qt::transparent);
    ui->customProgressbar->setBorder(true);
    ui->customProgressbar->setBorderRadius(5);
    //ui->optionFrame->hide();
    //setAttribute(Qt::WA_NoMousePropagation, false);
    //setAttribute(Qt::WA_MouseNoMask, true);

    connect(ui->userFrame,&PopoutFrame::hoverStarted,this,[this]{
        ui->logoutButton->show();
        ui->settingButton->show();
        ui->themeFrame->show();
    });
    connect(ui->userFrame,&PopoutFrame::hoverEnded,this,[this]{
        ui->logoutButton->hide();
        ui->settingButton->hide();
        ui->themeFrame->hide();
    });


    ui->logoutButton->hide();
    ui->settingButton->hide();
    ui->themeFrame->hide();


    m_assistant=new DynamicFrameAssistant(this);
    m_assistant->addChildWidget(TNotifaction::instance(),Qt::AlignHCenter | Qt::AlignTop);
    m_assistant->setChildWidgetMargin(TNotifaction::instance(),15);

    connect(TNotifaction::instance(),&TNotifaction::incomingNotif,this,[this]{
        m_assistant->popWidget(TNotifaction::instance(),true);
        QTimer::singleShot(5300,0,[this]{
            m_assistant->popWidget(TNotifaction::instance(),false);
        });
    });


    m_assistant_2=new DynamicFrameAssistant(ui->pFrame);

    m_assistant_2->addChildWidget(ui->userFrame,Qt::AlignRight | Qt::AlignTop);
    m_assistant_2->setChildWidgetMargin(ui->userFrame,5);


    ui->userFrame->setPopoutDirection(PopoutFrame::Bottom | PopoutFrame::Center);
    ui->userFrame->setPopoutMaxHeight(210);
    ui->userFrame->setPopoutScale(3.6);
    ui->userFrame->setWidthExpansionFactor(1);
    ui->userFrame->setCornerStyle(TFrame::CornerStyle::None);

    m_assistant->popAll(false);
    m_assistant->update();

    ui->stackedWidget->setBlurEffectEnabled(false);
    ui->stackedWidget->setAnimationType(TStackedWidget::HorizontalSlide);
    ui->stackedWidget->setCurve(QEasingCurve::OutQuad);

    setEnableBackground(true);
    setBackgroundImage(":/pictures/fullblurbluesky.png");
    setCornerStyle(TWidget::CornerStyle::None);

    ui->fileStack->setAnimationType(TStackedWidget::VerticalSlide);

    ui->recursiveSearchButton->setChecked(true);
    setCurrentWidget(m_loginPage);

    resize(900,600);

    setMarginDirection(Left);
    setMarginWidth(20);


    connect(this,&TWidget::mouseEnteredMarginArea,this,[this](){
        //m_assistant->popWidget(ui->buttonFrame,true);
        if(m_layout1->isAnimating() || m_layout1->isZoomed()){
            m_layout1->showAll();
            ui->dynamicLayout->setContentsMargins(5,1,5,2);
            ui->transactionFrameList->hide();
            ui->expandButton->setObjectName("expandButton");
            return;
        }
        _pop(true);

    });
    connect(ui->optionFrame,&HFrame::hoverEnded,this,[this]{
        //m_assistant->popWidget(ui->buttonFrame,false);
        //ui->buttonFrame->setBorderColor(Qt::white);
        if(m_layout1->isAnimating() || m_layout1->isZoomed()) return;
        _pop(false);
    });

    ui->expandButton->setObjectName("expandButton");
    connect(ui->expandButton,&QPushButton::clicked,this,[this]{
        if(!ui->optionFrame->isVisible()) return;
        m_layout1->zoomTo(ui->dynamicFrame);
        ui->dynamicLayout->setContentsMargins(5,5,5,5);
        ui->transactionFrameList->show();
        ui->expandButton->setObjectName("shrinkButton");
    });


    connect(BarUpdater::instance(),&BarUpdater::increased,this,[this]{
        int maxRange=BarUpdater::transactionCount()*10000;
        ui->customProgressbar->setRange(0,maxRange);
        int progressPercentage = static_cast<int>((BarUpdater::globalValue() * maxRange) / BarUpdater::globalSize());
        ui->customProgressbar->setValue(progressPercentage);
        QString progressText="Transaction " + ui->customProgressbar->text()+" completed...";
        ui->transactionLabel->setText(progressText);
    },Qt::QueuedConnection);


    ui->transactionFrameList->hide();


    m_loader=new LoadingFrame(this);
    m_setting=new Setting(nullptr,this);

    QThread *delta= new QThread(nullptr);

    m_connect= new NetworkAgent(smanager,nullptr);
    m_connect->moveToThread(delta);

    connect(delta,&QThread::started,m_connect,[this]{
        m_connect->build();
        m_connect->start(TWorker::High);
    });

    connect(delta,&QThread::finished,this,[this,delta]{
        delta->deleteLater();
        delete m_connect;
    });

    connect(m_connect,&NetworkAgent::connected,this,[](bool connected){
        if(connected) {
            qDebug("success connecting to server !!!!!");

        }else {
            qDebug("failed to connect to server");
            qWarning("Connection Failed!!!!");
            QApplication::exit(-5);
        }
    });

    m_loginPage=new loginPage(m_connect,ui->stackedWidget,ui->stackedWidget);
    m_signUpPage=new SignUpPage(m_connect,ui->stackedWidget,ui->stackedWidget);



    m_tManager=new ThreadManager(m_connect);

    ui->verticalLayout_3->setAlignment(Qt::AlignTop);
    m_fileManager=new TFileManager(m_tManager,m_connect,ui->verticalLayout_3,ui->prev,ui->next,ui->searchHoverButton,
                                     ui->searchLineEdit,ui->fileStack);



    connect(m_loginPage,&loginPage::Done,this,[this]{
        m_connect->userInit();
    });

    connect(m_loginPage,&loginPage::startLoading,this,&Home::load);
    connect(m_loginPage,&loginPage::stopLoading,this,&Home::unLoad);
    connect(m_signUpPage,&SignUpPage::startLoading,this,&Home::load);
    connect(m_signUpPage,&SignUpPage::stopLoading,this,&Home::unLoad);

    connect(m_connect,&NetworkAgent::initSuccess,this,[this]{
        m_fileManager->setup(m_connect->getList());

        m_settingManager=new SettingsManager(m_connect,m_connect->getUsername());
        m_setting->setManager(m_settingManager);

        connect(m_settingManager,&SettingsManager::backgroundImageChanged,this,[this](const QString&path){
            setBackgroundImage(path);
        });

        if(m_settingManager->customImage()) setBackgroundImage(m_settingManager->backgroundImage());
        if(m_settingManager->customColor()) setBackgroundImage(m_settingManager->backgroundColor());


        m_fileManager->makeTransparent(m_settingManager->transparency());

        connect(m_settingManager,&SettingsManager::transparencyChanged,this,&Home::makeTransparent);

        connect(m_settingManager,&SettingsManager::defaultProfilePictureChanged,this,[this](QPixmap p){
            ui->userIcon->setIcon(QIcon(p));
        });

        m_settingManager->updateProfilePicture(m_connect->getUserProfile());


        m_tManager->setManager(m_settingManager);
        //m_fileManager->setManager(m_sManager);

    });

    connect(m_fileManager,&TFileManager::doneSettingUpFileSys,this,[this]{
        QPixmap p;
        p.loadFromData(m_connect->getUserProfile());
        ui->userIcon->setIcon(QIcon(p));
        ui->usernameLabel->setText(TCLOUD::capitalize(m_connect->getUsername()));

        setCurrentWidget(ui->homePage);
        int cValue=(m_fileManager->usage()*10000)/m_connect->getAvailableSpace();
        ui->cprogressBar->setRange(0,10000);
        ui->cprogressBar->setValue(cValue);
        ui->cprogressBar->setCircularDegree(320);


        ui->usagelabel->setText(QString("Usage :%1").arg(TCLOUD::formatSize(m_fileManager->usage())));
        ui->availableSpaceLabel->setText(QString("Total Space :%1").arg(TCLOUD::formatSize(m_connect->getAvailableSpace())));

        unLoad();
        TCLOUD::Wait(156);
        TNotifaction::instance()->setMessage("Welcome "+TCLOUD::capitalize(m_connect->getUsername())+" 😊");

        initView(ui->tableView,TFileManager::baseFolder->getList());

    });

    connect(this,&TWidget::resizing,this,&Home::handleResize);

    connect(ui->closeButton,&QPushButton::clicked,this,&TWidget::close);
    connect(ui->homeButton,&QPushButton::clicked,m_fileManager,&TFileManager::showHome);
    connect(ui->trashButton,&QPushButton::clicked,m_fileManager,&TFileManager::showDeleted);
    connect(ui->usageButton,&QPushButton::clicked,this,[this]{
        ui->fileStack->setCurrentWidget(ui->usagePage);

    });
    connect(ui->settingButton,&QPushButton::clicked,this,[this]{
        setCurrentWidget(m_setting);
        setDetectionEnabled(false);
    });

    connect(m_setting,&Setting::aboutToClose,this,[this]{
        setCurrentWidget(ui->homePage);
        setDetectionEnabled(true);
    });


    connect(m_fileManager,&TFileManager::pathChanged,ui->historyWidget,&THistoryDisplay::setPath);


    connect(m_setting,&Setting::startLoading,this,&Home::load);
    connect(m_connect,&NetworkAgent::startLoading,this,&Home::load);
    connect(m_setting,&Setting::stopLoading,this,&Home::unLoad);
    connect(m_connect,&NetworkAgent::stopLoading,this,&Home::unLoad);


    connect(ui->userFrame,&TFrame::Clicked,ui->settingButton,&QPushButton::click);
    connect(ui->usernameLabel,&TLabel::Clicked,ui->settingButton,&QPushButton::click);
    connect(ui->userIcon,&QPushButton::clicked,ui->settingButton,&QPushButton::click);
    connect(ui->refreshButton,&QPushButton::clicked,this,&Home::refresh);


    delta->start(QThread::HighPriority);

    ui->stackedWidget->addWidget(m_setting);
    ui->stackedWidget->addWidget(m_signUpPage);
    ui->stackedWidget->addWidget(m_loginPage);

    ui->stackedWidget->setCurrentWidget(m_loginPage);

    InputDialog::Init(m_connect,this);
    InfoPage::Init(this);
    TClipBoard::Init();
    TFileChecker::Init();

    m_infoPage=InfoPage::instance();
    m_iDialog=InputDialog::instance();


    connect(ui->searchLineEdit,&TLineEdit::focusEntered,this,[this]{
        ui->searchFile->setStyleSheet(ui->searchFile->styleSheet().replace("#ccc","#469ef5"));
    });
    connect(ui->searchLineEdit,&TLineEdit::focusLeave,this,[this]{
        ui->searchFile->setStyleSheet(ui->searchFile->styleSheet().replace("#469ef5","#ccc"));
    });

    connect(ui->recursiveSearchButton,&ToggleButton::clicked,m_fileManager,&TFileManager::setRecursiveSearch);

    connect(ui->recursiveSearchButton,&ToggleButton::clicked,this,[](bool on){
        if(on){
            TNotifaction::instance()->setMessage(" Recursive search enabled ");
        }else{
            TNotifaction::instance()->setMessage(" Recursive search disabled ");
        }
    });

    connect(ui->historyWidget,&THistoryDisplay::pathClicked,m_fileManager,&TFileManager::gotoFolder);
    connect(ui->historyWidget,&THistoryDisplay::middleClicked,m_fileManager,&TFileManager::gotoFolder);


}

Home::~Home()
{
    delete ui;
    if(m_layout1) delete m_layout1;
    m_connect->stop();

    delete m_loader;
    delete m_fileManager;
    delete m_tManager;
    delete m_assistant;
    delete m_assistant_2;


    InfoPage::cleanUp();
    InputDialog::cleanUp();
    TClipBoard::cleanUp();
    TFileChecker::cleanUp();
    TNotifaction::cleanUp();
    BarUpdater::cleanUp();

}

void Home::load(const QString &text)
{
    if(m_loader->isLoading()){
        m_loader->setMessage(text);
        return;
    }
    m_loader->start(rect(),text);
}

void Home::unLoad()
{
    m_loader->stop();
}

void Home::refresh()
{
    load("Refreshing please wait..... ⏳");
    m_fileManager->refresh();
    m_setting->breakLogic();
    m_connect->userInit();

    wasRefresh=true;
}

void Home::setBackgroundImage(const QString &img)
{
    //ui->dynamicFrame->setBackgroundImage(img);
    QString newPath=img;
    if(!QFile::exists(newPath)){
        newPath=":/pictures/fullblurbluesky.png";
    }
    TWidget::setBackgroundImage(newPath);

}

void Home::setCurrentWidget(QWidget *widget)
{
    if(!widget) return;
    if(widget==ui->homePage){
        //m_assistant->popWidget(TNotifaction::instance(),true);
        //m_assistant->setBlock(true);
        //ui->gridLayout_2->addWidget(TNotifaction::instance(),1,0,1,2,Qt::AlignHCenter);
    }else{
        //m_assistant->setBlock(false);
        //ui->gridLayout_2->removeWidget(TNotifaction::instance());
        resize(width(),height());
        //m_assistant->popWidget(TNotifaction::instance(),false);
    }
    ui->stackedWidget->setCurrentWidget(widget);
    m_layout1->update();

    //qDebug() << "assistant blocked="<< m_assistant->childWidgetBlocked();
}

void Home::initView(QTableView *tableView, QList<eltCore> files)
{
    if(wasRefresh) return;

    // Create model with improved headers
    auto *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"📁 Name", "📊 Size", "📅 Modified"});

    QFileIconProvider iconProvider;

    for (const eltCore &core : std::as_const(files)) {
        // Skip directories (items without extensions)
        if(core.name.split(".").size() == 1) continue;

        // Clean up the filepath - remove :/ prefix
        QString cleanPath = core.info.filepath;
        if(cleanPath.startsWith(":/")) {
            cleanPath = cleanPath.mid(2);
        }

        // Create row items
        auto *nameItem = new QStandardItem(cleanPath);
        auto *sizeItem = new QStandardItem(TCLOUD::formatSize(core.info.size));
        auto *dateItem = new QStandardItem(core.info.lastEditDate.toString("MMM dd, yyyy • hh:mm"));

        // Set icon for the name item
        nameItem->setIcon(iconProvider.icon(QFileInfo(core.name)));

        // Make size and date items non-editable and center-aligned
        sizeItem->setFlags(sizeItem->flags() & ~Qt::ItemIsEditable);
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        sizeItem->setTextAlignment(Qt::AlignCenter);
        dateItem->setTextAlignment(Qt::AlignCenter);

        // Add subtle styling data for potential use with delegates
        nameItem->setData(QVariant::fromValue(core.name), Qt::UserRole);
        sizeItem->setData(QVariant::fromValue(core.info.size), Qt::UserRole);
        dateItem->setData(QVariant::fromValue(core.info.lastEditDate), Qt::UserRole);

        // Create and append row
        QList<QStandardItem *> row;
        row << nameItem << sizeItem << dateItem;
        model->appendRow(row);
    }

    // Set model and configure table view
    tableView->setModel(model);

    // Modern table styling
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->verticalHeader()->setVisible(false);
    tableView->setShowGrid(false);
    tableView->setSortingEnabled(true);

    // Configure column resizing
    QHeaderView *header = tableView->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    // Set minimum column widths for better appearance
    header->setMinimumSectionSize(120);
    tableView->setColumnWidth(1, 100);
    tableView->setColumnWidth(2, 150);
}

void Home::pop(bool show) {
    if (show) {
        if (ui->optionFrame->isVisible()) {
            return;
        }

        handleShow();
    } else {
        handleHide();
    }

    update();
}

void Home::makeTransparent(bool t)
{
    m_fileManager->makeTransparent(t);
    TNotifaction* ins=TNotifaction::instance();
    ins->setMessage("Transparency changed successfully");
}

void Home::_pop(bool show) {
    static bool isAnimating = false;
    if (isAnimating) return;

    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);

    isAnimating = true;
    short duration=300;

    auto onFinished = [group]() {
        group->deleteLater();
        isAnimating = false;
    };

    QMargins margins = m_layout1->contentsMargins();
    int spacing = m_layout1->spacing();
    int spacingH = m_layout1->spacing();
    QRect fullRect = this->rect().adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom());
    update();


    if (show) {
        if (ui->optionFrame->isVisible()) {
            isAnimating = false;
            group->deleteLater();
            return;
        }

        ui->optionFrame->show();

        int optionW = ui->optionFrame->width();
        QRect optionFrameEnd(fullRect.left(), fullRect.top(), optionW, fullRect.height());
        QRect optionFrameStart(-optionW, fullRect.top(), optionW, fullRect.height());

        int pFrameW = fullRect.width() - optionW - spacing;
        int pFrameH = fullRect.height() -ui->dynamicFrame->height()- spacingH;
        QRect pFrameEnd(fullRect.x()+optionW+spacing, fullRect.top(), pFrameW, pFrameH);
        QRect frame2End(fullRect.x()+optionW+spacing, ui->dynamicFrame->y(), pFrameW,ui->dynamicFrame->height());

        //ui->optionFrame->setGeometry(optionFrameStart);
        ui->optionFrame->raise();

        QPropertyAnimation* optionAnim = new QPropertyAnimation(ui->optionFrame, "geometry");
        optionAnim->setDuration(duration);
        optionAnim->setEasingCurve(QEasingCurve::OutQuad);
        optionAnim->setStartValue(optionFrameStart);
        optionAnim->setEndValue(optionFrameEnd);
        group->addAnimation(optionAnim);

        QPropertyAnimation* pFrameAnim = new QPropertyAnimation(ui->pFrame, "geometry");
        pFrameAnim->setDuration(duration);
        pFrameAnim->setEasingCurve(QEasingCurve::OutQuad);
        //pFrameAnim->setStartValue(pFrameStart);
        pFrameAnim->setEndValue(pFrameEnd);
        group->addAnimation(pFrameAnim);

        QPropertyAnimation* frame_2Anim = new QPropertyAnimation(ui->dynamicFrame, "geometry");
        frame_2Anim->setDuration(duration);
        frame_2Anim->setEasingCurve(QEasingCurve::OutQuad);
        //pFrameAnim->setStartValue(pFrameStart);
        frame_2Anim->setEndValue(frame2End);
        group->addAnimation(frame_2Anim);

        connect(group, &QParallelAnimationGroup::finished, this, [=]() {
            handleShow();
            onFinished();
        });

    } else {
        if (!ui->optionFrame->isVisible()) {
            isAnimating = false;
            group->deleteLater();
            return;
        }

        QRect optionFrameStart = ui->optionFrame->geometry();
        QRect optionFrameEnd(optionFrameStart);
        optionFrameEnd.moveLeft(-optionFrameStart.width());

        int pFrameH = fullRect.height() -ui->dynamicFrame->height()- spacingH;
        QRect pFrameEnd(fullRect.left(), fullRect.top(), fullRect.width(), pFrameH);
        QRect frame2End(fullRect.left(), ui->dynamicFrame->y(), fullRect.width(),ui->dynamicFrame->height());

        QPropertyAnimation* optionAnim = new QPropertyAnimation(ui->optionFrame, "geometry");
        optionAnim->setDuration(duration);
        optionAnim->setEasingCurve(QEasingCurve::OutQuad);
        optionAnim->setStartValue(optionFrameStart);
        optionAnim->setEndValue(optionFrameEnd);
        group->addAnimation(optionAnim);

        QPropertyAnimation* pFrameAnim = new QPropertyAnimation(ui->pFrame, "geometry");
        pFrameAnim->setDuration(duration);
        pFrameAnim->setEasingCurve(QEasingCurve::OutQuad);
        //pFrameAnim->setStartValue(pFrameStart);
        pFrameAnim->setEndValue(pFrameEnd);
        group->addAnimation(pFrameAnim);

        QPropertyAnimation* frame2Anim = new QPropertyAnimation(ui->dynamicFrame, "geometry");
        frame2Anim->setDuration(duration);
        frame2Anim->setEasingCurve(QEasingCurve::OutQuad);
        //pFrameAnim->setStartValue(pFrameStart);
        frame2Anim->setEndValue(frame2End);
        group->addAnimation(frame2Anim);

        connect(group, &QParallelAnimationGroup::finished, this, [=]() {
            handleHide();
            onFinished();
        });
    }

    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void Home::handleShow() {
    ui->optionFrame->show();
    m_layout1->addWidget(ui->optionFrame, 0, 0, 2, 1);
    m_layout1->addWidget(ui->pFrame, 0, 1, 1, 1);
    m_layout1->addWidget(ui->dynamicFrame, 1, 1, 1, 1);
}

void Home::handleHide()
{
    m_layout1->removeWidget(ui->optionFrame);

    m_layout1->addWidget(ui->pFrame, 0, 0, 1, 2);
    m_layout1->addWidget(ui->dynamicFrame, 1, 0, 1, 2);
    ui->optionFrame->hide();
}


void Home::handleResize() {
    QRect rect = this->rect();

    m_loader->setGeometry(rect);

    int x1 = rect.center().x() - m_infoPage->width() / 2;
    int y1 = rect.center().y() - m_infoPage->height() / 2;
    int x2 = rect.center().x() - m_iDialog->width() / 2;
    int y2 = rect.center().y() - m_iDialog->height() / 2;

    m_infoPage->move(x1, y1);
    m_infoPage->raise();
    m_iDialog->move(x2, y2);
    m_iDialog->raise();
    //ui->searchFrame->resize(ui->pFrame->width()/3,ui->searchFile->height());


    //QRect rect2 = ui->pFrame->rect();
    //TNotifaction::instance()->setPopoutMaxWidth(rect.width() - 10);

    //pop(!(rect.width() < 880));
}
