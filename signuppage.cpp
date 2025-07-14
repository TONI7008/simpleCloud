#include "signuppage.h"
#include "ui_signuppage.h"

#include "networkagent.h"
#include "tstackedwidget.h"
#include "tmessagebox.h"
#include "inputdialog.h"
#include "tnotification.h"


SignUpPage::SignUpPage(NetworkAgent* con,TStackedWidget* stack,QWidget *parent)
    : TWidget(parent)
    , ui(new Ui::SignUpPage),
    m_thread(con),
    m_stack(stack)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setEnableBackground(!true);
    setBackgroundImage(":/Images/image.png");
    setBorderRadius(15);

    ui->password->setEchoMode(QLineEdit::Password);
    ui->password2->setEchoMode(QLineEdit::Password);

    connect(ui->showCheckbox, &QCheckBox::clicked, this, [&]{
        if(ui->password->echoMode() == QLineEdit::Password){
            ui->password->setEchoMode(QLineEdit::Normal);
        } else {
            ui->password->setEchoMode(QLineEdit::Password);
        }
    });

    connect(m_thread, &NetworkAgent::registerSuccess, this, [&]{
        stop();
    });
    connect(m_thread, &NetworkAgent::registerContinue, this, [&]{
        stop();
        InputDialog* dialog=InputDialog::instance();
        dialog->setPurpose(InputDialog::AuthentificationCode);
        QEventLoop loop;
        connect(dialog,&InputDialog::codeReady,&loop,[&](QString verifNumber){
            QString usr=ui->username->text();
            QString passw=ui->password->text();
            QString email=ui->email->text();
            m_thread->signUpConfirm(usr,email,passw,verifNumber);
            loop.quit();
        });

        dialog->show();
        loop.exec();
    });
    connect(m_thread, &NetworkAgent::registerFailed, this, [&](QString error){
        TNotifaction::instance()->setMessage(error,true);
        stop();
    });
    connect(ui->loginButton,&QPushButton::clicked,this,[this]{
        m_stack->setCurrentIndex(5);
    });
    connect(ui->signButton,&QPushButton::clicked,this,&SignUpPage::signUp);


    connect(ui->closeButton,&QPushButton::clicked,this,[this]{
        m_stack->parentWidget()->close();
    });

}

SignUpPage::~SignUpPage()
{
    delete ui;
}

void SignUpPage::start(const QString& mes)
{

    emit startLoading(mes);
}

void SignUpPage::stop(){
    emit stopLoading();
}

void SignUpPage::signUp()
{
    QString usr=ui->username->text();
    QString passw=ui->password->text();
    QString email=ui->email->text();
    if(usr.isEmpty() || passw.isEmpty() || email.isEmpty()){
        TMessageBox b;
        b.critical(this,"missing data","username or password not provided");
        b.show();
        return;
    }
    if(ui->password2->text()!=passw){
        TMessageBox b;
        b.critical(this,"wrong password","passwords dont match");
        stop();
        return;
    }
    start("signing up.....");
    m_thread->signUp(usr,email,passw);
}
