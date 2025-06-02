#include "home.h"
#include "ui_home.h"
#include "infopage.h"
#include "inputdialog.h"

#include "tfilemanager.h"
#include "loadingframe.h"
#include "mainthread.h"
#include "tsecuritymanager.h"
#include "setting.h"
#include "settingmanager.h"
#include "threadmanager.h"
#include "tcloud.h"
#include "tfilechecker.h"
#include "tclipboard.h"
#include "loginpage.h"
#include "signuppage.h"

Home::Home(TSecurityManager* smanager,QWidget *parent)
    : TWidget(parent)
    , ui(new Ui::Home)
{
    ui->setupUi(this);
    ui->userFrame->setEnableBackground(false);
    ui->userFrame->setEnableBackground(false);


    ui->userFrame->setCornerStyle(TFrame::CornerStyle::None);

    ui->stackedWidget->setBlurEffectEnabled(false);
    ui->stackedWidget->setAnimationType(TStackedWidget::VerticalSlide);

    m_loader=new LoadingFrame(this);
    m_setting=new Setting(nullptr,this);

    QThread *delta= new QThread(nullptr);

    m_connect= new mainThread(smanager,nullptr);
    m_connect->moveToThread(delta);

    connect(delta,&QThread::started,m_connect,[this]{
        m_connect->build();
        m_connect->start(TWorker::High);
    });

    connect(delta,&QThread::finished,this,[this,delta]{
        delta->deleteLater();
        delete m_connect;
    });

    connect(m_connect,&mainThread::connected,this,[](bool connected){
        if(connected) {
            qDebug("success connecting to server!!!!!");
        }else {
            qDebug("failed to connect to server");
            qWarning("Connection Failed!!!!");
            QApplication::exit(-1);
        }
    });

    m_loginPage=new loginPage(m_connect,ui->stackedWidget,ui->stackedWidget);
    m_signUpPage=new SignUpPage(m_connect,ui->stackedWidget,ui->stackedWidget);



    m_tManager=new ThreadManager(m_connect);

    ui->verticalLayout_3->setAlignment(Qt::AlignTop);
    m_fileManager=new TFileManager(m_tManager,m_connect,ui->verticalLayout_3,ui->prev,ui->next,ui->searchHoverButton,ui->searchLineEdit,ui->fileStack);



    connect(m_loginPage,&loginPage::Done,this,[this]{
        m_connect->userInit();
    });

    connect(m_loginPage,&loginPage::startLoading,this,&Home::load);
    connect(m_loginPage,&loginPage::stopLoading,this,&Home::unLoad);
    connect(m_signUpPage,&SignUpPage::startLoading,this,&Home::load);
    connect(m_signUpPage,&SignUpPage::stopLoading,this,&Home::unLoad);

    connect(m_connect,&mainThread::initSuccess,this,[this]{
        m_fileManager->setupAsync(m_connect->getList());

        m_settingManager=new SettingsManager(m_connect,m_connect->getUsername());
        m_setting->setManager(m_settingManager);

        connect(m_settingManager,&SettingsManager::backgroundImageChanged,this,[this](const QString&path){
            setBackgroundImage(path);
        });

        if(m_settingManager->customImage()) setBackgroundImage(m_settingManager->backgroundImage());
        if(m_settingManager->customColor()) setBackgroundImage(m_settingManager->backgroundColor());


        m_fileManager->makeTransparent(m_settingManager->transparency());

        connect(m_settingManager,&SettingsManager::transparencyChanged,m_fileManager,&TFileManager::makeTransparent);

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

        ui->stackedWidget->setCurrentWidget(ui->homePage);
        int cValue=(m_fileManager->usage()*10000)/m_connect->getAvailableSpace();
        ui->cprogressBar->setRange(0,10000);
        ui->cprogressBar->setValue(cValue);
        ui->cprogressBar->setCircularDegree(320);


        ui->usagelabel->setText(QString("Usage :%1").arg(TCLOUD::formatSize(m_fileManager->usage())));
        ui->availableSpaceLabel->setText(QString("Total Space :%1").arg(TCLOUD::formatSize(m_connect->getAvailableSpace())));
        ui->dynamicFrame->setMaxwidth(ui->pFrame->width());
        unLoad();

        ui->dynamicFrame->setMessage("Welcome "+m_connect->getUsername()+" 😊");
    });
    connect(this,&TWidget::resizing,this,&Home::handleResize);

    connect(ui->closeButton,&QPushButton::clicked,this,&TWidget::close);
    connect(ui->homeButton,&QPushButton::clicked,m_fileManager,&TFileManager::showHome);
    connect(ui->trashButton,&QPushButton::clicked,m_fileManager,&TFileManager::showDeleted);
    connect(ui->usageButton,&QPushButton::clicked,this,[this]{
        ui->fileStack->setCurrentWidget(ui->usagepage);
    });
    connect(ui->settingButton,&QPushButton::clicked,this,[this]{
        ui->stackedWidget->setCurrentWidget(m_setting);
        //m_setting->showUserInfo();
    });

    connect(m_setting,&Setting::aboutToClose,this,[this]{
        ui->stackedWidget->setCurrentWidget(ui->homePage);
    });


    connect(m_fileManager,&TFileManager::pathChanged,ui->dynamicFrame,&TDynamicFrame::setPath);


    connect(m_setting,&Setting::startLoading,this,&Home::load);
    connect(m_connect,&mainThread::startLoading,this,&Home::load);
    connect(m_setting,&Setting::stopLoading,this,&Home::unLoad);
    connect(m_connect,&mainThread::stopLoading,this,&Home::unLoad);


    connect(ui->userFrame,&TFrame::Clicked,ui->settingButton,&QPushButton::click);
    connect(ui->usernameLabel,&TLabel::Clicked,ui->settingButton,&QPushButton::click);
    connect(ui->userIcon,&QPushButton::clicked,ui->settingButton,&QPushButton::click);
    connect(ui->refreshButton,&QPushButton::clicked,this,&Home::refresh);


    delta->start(QThread::HighPriority);

    ui->stackedWidget->addWidget(m_setting);
    ui->stackedWidget->addWidget(m_signUpPage);
    ui->stackedWidget->addWidget(m_loginPage);

    InputDialog::Init(m_connect,this);
    InfoPage::Init(this);
    TClipBoard::Init();
    TFileChecker::Init();

    m_infoPage=InfoPage::instance();
    m_iDialog=InputDialog::instance();

    setEnableBackground(true);
    setBackgroundImage(":/pictures/fullblurbluesky.png");
    setCornerStyle(TWidget::CornerStyle::None);

    ui->fileStack->setAnimationType(TStackedWidget::HorizontalSlide);

    connect(ui->searchLineEdit,&TLineEdit::focusEntered,this,[this]{
        ui->searchFile->setStyleSheet(ui->searchFile->styleSheet().replace("#ccc","#469ef5"));
    });
    connect(ui->searchLineEdit,&TLineEdit::focusLeave,this,[this]{
        ui->searchFile->setStyleSheet(ui->searchFile->styleSheet().replace("#469ef5","#ccc"));
    });

    connect(ui->recursiveSearchButton,&ToggleButton::clicked,m_fileManager,&TFileManager::setRecursiveSearch);

    connect(ui->recursiveSearchButton,&ToggleButton::clicked,this,[this](bool on){
        if(on){
            ui->dynamicFrame->setMessage(" Recursive search enabled ");
        }else{
            ui->dynamicFrame->setMessage(" Recursive search disabled ");
        }
    });

    ui->recursiveSearchButton->setChecked(true);
    ui->stackedWidget->setCurrentWidget(m_loginPage);

    resize(900,600);
}

Home::~Home()
{
    delete ui;
    m_connect->stop();

    delete m_loader;
    delete m_fileManager;
    delete m_tManager;


    InfoPage::cleanUp();
    InputDialog::cleanUp();
    TClipBoard::cleanUp();
    TFileChecker::cleanUp();

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
}

void Home::setBackgroundImage(const QString &img)
{
    ui->dynamicFrame->setBackgroundImage(img);

    TWidget::setBackgroundImage(img);
}

void Home::handleResize()
{
    QRect rect=this->rect();

    m_loader->setGeometry(rect);

    int x1=rect.center().x()-m_infoPage->width()/2;
    int y1=rect.center().y()-m_infoPage->height()/2;
    int x2=rect.center().x()-m_iDialog->width()/2;
    int y2=rect.center().y()-m_iDialog->height()/2;

    m_infoPage->move(x1, y1);
    m_infoPage->raise();
    m_iDialog->move(x2, y2);
    m_iDialog->raise();

    QRect rect2=ui->pFrame->rect();
    ui->dynamicFrame->setMaxwidth(rect2.width()-5);
    ui->searchFile->setMinimumWidth(rect2.width()/3.4);
}

