#include "inputdialog.h"
#include "ui_inputdialog.h"
#include "mainthread.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>

InputDialog* InputDialog::m_dialog=nullptr;

InputDialog::InputDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setAnimationType(TStackedWidget::HorizontalSlide);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    raise();

    //setBackgroundImage(":/pictures/image2.png");
    //setEnableBackground(true);
    //setBorder(true);
    //setBorderRadius(15);
    //setBorderSize(5);

    m_purpose=Rename;
    hide();

    connect(ui->doneButton,&QPushButton::clicked,this,&InputDialog::processFolderCreation);
    connect(ui->doneButton_3,&QPushButton::clicked,this,&InputDialog::processRenaming);
    connect(ui->closeButton,&QPushButton::clicked,this,[this]{
        cancel();
    });
    connect(ui->cancelButton_3,&QPushButton::clicked,this,[this]{
        cancel();
    });
    connect(ui->cancelButton,&QPushButton::clicked,this,[this]{
        cancel();
    });

    QRegularExpressionValidator noSlashesValidator(QRegularExpressionValidator(QRegularExpression("[^/\\\\]*")));

    ui->renameLineEdit->setValidator(&noSlashesValidator);
    ui->folderLineEdit->setValidator(&noSlashesValidator);


    setMaximumSize(400,225);
}

InputDialog::InputDialog(mainThread *mainthread, QWidget *parent) : InputDialog(parent)
{
    m_connect=mainthread;
}


InputDialog::~InputDialog()
{
    delete ui;
}

void InputDialog::processFolderCreation()
{
    if(!m_connect) return;
    if(m_data.size()!=1) return;

    const QString name=ui->folderLineEdit->text().trimmed();
    QString tmp=m_data.at(0)+"/"+name+"/.";

    m_connect->createDir(tmp);

    QEventLoop loop;
    connect(m_connect,&mainThread::createDirSuccess,&loop,[this,&name,&loop]{
        loop.quit();
        qDebug() << "name :" << name;
        emit DoneCreatingFolder(name);
    });
    connect(m_connect,&mainThread::createDirFailed,&loop,[this,&loop](QString error){
        qDebug() << "error="<<error;
        cancel();
        loop.quit();
    });
    loop.exec();
    m_data.clear();
    hide();
}

void InputDialog::processRenaming()
{
    if(ui->renameLineEdit->text().isEmpty()) emit canceling();
    if(!m_connect) return;
    if(m_data.size()!=4) return;

    const QString name=ui->renameLineEdit->text().trimmed();

    QString oldP = m_data.at(0);
    QString newP = oldP;
    newP.replace(m_data.at(1), name);

    bool isfolder=m_data.at(2)=="1";
    bool isempty=m_data.at(3)=="1";

    if(isfolder){
        if(isempty){
            m_connect->renameFolder(oldP+"/.",newP+"/.");
        }else{
            m_connect->renameFolder(oldP,newP);
        }
    }else{
        m_connect->renameFile(oldP,newP);
    }

    QEventLoop loop;
    connect(m_connect,&mainThread::renameSuccess,&loop,[this,&name,&loop]{
        loop.quit();
        qDebug() << "name :" << name;
        m_data.clear();
        emit DoneRenaming(name);
    });
    connect(m_connect,&mainThread::renameFailed,&loop,[this,&loop](QString error){
        qDebug() << "error renamed failed="<<error;
        cancel();
        loop.quit();
    });

    m_data.clear();
    loop.exec();
}

void InputDialog::cancel()
{
    emit canceling();
    m_data.clear();
    hide();
}

InputDialog::Purpose InputDialog::getPurpose() const
{
    return m_purpose;
}

InputDialog *InputDialog::instance()
{
    if(!m_dialog){
        return nullptr;
    }
    return m_dialog;
}

void InputDialog::Init(mainThread* th,QWidget* parent)
{
    if(m_dialog) return;

    m_dialog=new InputDialog(th,parent);
}

void InputDialog::cleanUp()
{
    if(m_dialog){
        m_dialog->deleteLater();
        m_dialog=nullptr;
    }
}

void InputDialog::setPurpose(Purpose newPurpose)
{
    m_purpose = newPurpose;

    switch (m_purpose) {
    case Rename:
        ui->stackedWidget->setCurrentWidget(ui->renamePage);
        break;
    case CreateFolder:
        ui->stackedWidget->setCurrentWidget(ui->createFolderPage);
        break;
    case AuthentificationCode:
        ui->stackedWidget->setCurrentWidget(ui->AuthentificationPage);
        break;
    default:
        break;
    }
}


InputDialog &InputDialog::operator<<(const QString &value)
{
    m_data << value;
    if(m_data.size()==3&& m_purpose==Rename){
        ui->renameLineEdit->setText(m_data.at(0));
    }
    return *this;
}
