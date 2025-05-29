#include "loginpage.h"
#include "ui_loginpage.h"

#include "loginpage.h"
#include "ui_loginpage.h"
#include "tstackedwidget.h"
#include "mainthread.h"

#include <QToolBar>
#include<QMessageBox>

loginPage::loginPage(mainThread* con,TStackedWidget* stack,QWidget *parent)
    : TWidget(parent)
    , ui(new Ui::loginPage),
    m_thread(con),
    m_stack(stack)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setEnableBackground(true);
    setBackgroundImage(":/Images/image.png");

    setBorderRadius(15);

    ui->password->setEchoMode(QLineEdit::Password);

    connect(ui->showCheckbox, &QCheckBox::clicked, this, [&]{
        if(ui->password->echoMode() == QLineEdit::Password){
            ui->password->setEchoMode(QLineEdit::Normal);
        } else {
            ui->password->setEchoMode(QLineEdit::Password);
        }
    });
    connect(m_thread, &mainThread::loginSuccess, this, [&]{
        stop();
        m_thread->setUsername(username());
        clean();
        emit Done();
    });
    connect(m_thread, &mainThread::loginFailed, this, [&](QString error){
        showMessage(error);
        stop();
    });

    connect(ui->loginButton,&QPushButton::clicked,this,&loginPage::login);
    connect(ui->signButton,&QPushButton::clicked,this,[this]{
        m_stack->setCurrentIndex(2);
    });

    connect(ui->toolButton,&QToolButton::clicked,this,[this]{
        m_stack->parentWidget()->close();
    });
    connect(this,&TWidget::debugPressed,this,[this]{
        m_stack->setCurrentIndex(3);
    });


}

loginPage::~loginPage()
{
    delete ui;
}

QString loginPage::username()
{
    return ui->username->text();
}

void loginPage::stop()
{

}

void loginPage::login(){
    QString usr=ui->username->text();
    QString passw=ui->password->text();
    if(usr.isEmpty() || passw.isEmpty()){
        QMessageBox b;
        b.setAttribute(Qt::WA_TranslucentBackground);
        b.critical(this,"missing data","username or password not provided");
        b.show();
        return;
    }
    emit startLoading("trying to log you in");
    m_thread->login(usr,passw);
}


void loginPage::showMessage(QString msg) {

    QWidget *messageWidget = new QWidget(m_stack->parentWidget());
    messageWidget->setStyleSheet("background:rgba(31,31,31,0.6);"
                                 "border-radius:8px;");
    QHBoxLayout *layout = new QHBoxLayout(messageWidget);
    QLabel *label = new QLabel(msg);

    messageWidget->setMaximumHeight(50);
    messageWidget->setMinimumHeight(50);
    messageWidget->setMinimumWidth(500);

    //messageWidget->setAttribute(Qt::WA_TranslucentBackground);

    label->setStyleSheet("color:red;"
                         "font: 700 italic 13pt \"Segoe UI\";"
                         "background:transparent;");

    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(spacer);

    QToolBar *toolBar = new QToolBar;
    toolBar->setStyleSheet("background: transparent;");
    layout->addWidget(toolBar);

    QToolButton *closeButton = new QToolButton;
    closeButton->setIcon(QIcon("/icons/close.png")); // Remplacez le chemin par celui de votre icône
    closeButton->setFixedSize(24, 24);
    closeButton->setStyleSheet("background:transparent;");

    connect(closeButton, &QToolButton::clicked,this,[=] {
        _pos-=60;
        messageWidget->setVisible(false);
        messageWidget->close();
        delete messageWidget;
    });

    toolBar->addWidget(closeButton);


    messageWidget->show();

    QPropertyAnimation *animation = new QPropertyAnimation(messageWidget, "pos");
    animation->setDuration(300); // Adjust duration as needed
    animation->setStartValue(QPoint(0, messageWidget->y()));
    animation->setEndValue(QPoint(0, messageWidget->y() + _pos));
    animation->setEasingCurve(QEasingCurve::OutQuad);

    animation->start();
    connect(animation,&QPropertyAnimation::finished,this,[=]{
        animation->deleteLater();
        messageWidget->raise();
    });

    _pos+=60;
}

void loginPage::clean(){
    ui->username->clear();
    ui->password->clear();
}



