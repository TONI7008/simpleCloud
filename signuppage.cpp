#include "signuppage.h"
#include "ui_signuppage.h"

#include "mainthread.h"
#include "tstackedwidget.h"
#include "tcloud.h"


SignUpPage::SignUpPage(mainThread* con,TStackedWidget* stack,QWidget *parent)
    : TWidget(parent)
    , ui(new Ui::SignUpPage),
    m_thread(con),
    m_stack(stack)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setEnableBackground(true);
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

    connect(m_thread, &mainThread::registerSuccess, this, [&]{
        stop();
    });
    connect(m_thread, &mainThread::registerContinue, this, [&]{
        stop();
    });
    connect(m_thread, &mainThread::registerFailed, this, [&](QString error){
        qDebug() << "error : " << error;
        stop();
    });
    connect(ui->loginButton,&QPushButton::clicked,this,[this]{
        m_stack->setCurrentIndex(1);
    });
    connect(ui->signButton,&QPushButton::clicked,this,&SignUpPage::signUp);

    connect(this,&TWidget::debugPressed,this,[this]{
        m_stack->setCurrentIndex(3);
    });

    connect(ui->toolButton,&QToolButton::clicked,this,[this]{
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
        QMessageBox b;
        b.setAttribute(Qt::WA_TranslucentBackground);
        b.critical(this,"missing data","username or password not provided");
        b.show();
        return;
    }
    if(ui->password2->text()!=passw){
        QMessageBox b;
        b.setAttribute(Qt::WA_TranslucentBackground);
        b.critical(this,"wrong password","passwords dont match");
        stop();
        return;
    }
    start("signing up.....");
    m_thread->signUp(usr,email,passw);
}
