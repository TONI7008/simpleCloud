#include "inputdialog.h"
#include "ui_inputdialog.h"
#include "networkagent.h"
#include "tnotification.h"
#include "tmessagebox.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>

InputDialog* InputDialog::m_dialog=nullptr;

InputDialog::InputDialog(QWidget *parent)
    : TDialog(parent)
    , ui(new Ui::InputDialog)
{
    ui->setupUi(this->contentWidget());
    ui->stackedWidget->setAnimationType(TStackedWidget::HorizontalSlide);

    raise();

    setBackgroundImage(":/pictures/image0.png");
    setEnableBackground(true);
    setBorder(true);
    setBorderRadius(25);
    setBorderSize(5);

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
    connect(ui->verifyButton,&QPushButton::clicked,this,[this]{
        if(ui->verificationLineEdit->text().isEmpty()){
            TMessageBox::critical(this,"Error","make sure to enter the code");
        }else{
            emit codeReady(ui->verificationLineEdit->text());
            cancel();
        }
    });

    QRegularExpressionValidator noSlashesValidator(QRegularExpressionValidator(QRegularExpression("[^/\\\\]*")));

    ui->renameLineEdit->setValidator(&noSlashesValidator);
    ui->folderLineEdit->setValidator(&noSlashesValidator);


    setMaximumSize(400,225);
}

InputDialog::InputDialog(NetworkAgent *NetworkAgent, QWidget *parent) : InputDialog(parent)
{
    m_connect=NetworkAgent;
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

    this->setDisabled(true);
    QEventLoop loop;
    connect(m_connect,&NetworkAgent::createDirSuccess,&loop,[this,&name,&loop]{
        qDebug() << "name :" << name;
        emit DoneCreatingFolder(name);
        loop.quit();
    });
    connect(m_connect,&NetworkAgent::createDirFailed,&loop,[this,&loop](QString error){
        TNotifaction::instance()->setMessage("Error :"+error,true);
        cancel();
        loop.quit();
    });
    loop.exec();
    m_data.clear();
    hide();
    this->setDisabled(false);
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
    this->setDisabled(true);

    QEventLoop loop;
    connect(m_connect,&NetworkAgent::renameSuccess,&loop,[this,&name,&loop]{
        qDebug() << "name :" << name;
        m_data.clear();
        emit DoneRenaming(name);
        loop.quit();
    });
    connect(m_connect,&NetworkAgent::renameFailed,&loop,[this,&loop](QString error){
        TNotifaction::instance()->setMessage("Error renamed failed="+error,true);
        cancel();
        loop.quit();
    });

    m_data.clear();
    loop.exec();
    this->setDisabled(false);
    hide();
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

void InputDialog::Init(NetworkAgent* th,QWidget* parent)
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
        ui->renameLineEdit->setText(m_data.at(1));
    }
    return *this;
}
