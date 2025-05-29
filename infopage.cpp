#include "infopage.h"
#include "tcloud.h"

#include <QFileIconProvider>
#include <QFileInfo>
#include "hoverbutton.h"
#include "tlabel.h"

InfoPage *InfoPage::m_infoPage=nullptr;

void InfoPage::changeInfo(TFileInfo info) {
    this->info = info;
    updateInfo();
}

void InfoPage::Init(QWidget *parent)
{
    if(m_infoPage) return;

    m_infoPage=new InfoPage(parent);
}

InfoPage::~InfoPage() {
    delete filePath;
    delete fileSize;
    delete date;
    delete uploadDate;
    delete iconButton;
}

void InfoPage::cleanUp() {
    if (m_infoPage) {
        m_infoPage->deleteLater();
        m_infoPage = nullptr;
    }
}

QIcon InfoPage::getIcon(){
    QFileIconProvider provider;
    QFileInfo inf(name());
    return provider.icon(inf);
}

QString InfoPage::name()
{
    QStringList names=info.filepath.split("/");
    return names.last();
}

InfoPage* InfoPage::instance()
{
    if (!m_infoPage) {
        return nullptr;
    }
    return m_infoPage;
}

void InfoPage::showForFile(const TFileInfo& info)
{
    this->info = info;
    m_folder = false;
    updateInfo();
    show();
    raise();
    activateWindow();
}
void InfoPage::showForFolder(const TFileInfo& info)
{
    this->info = info;
    m_folder = true;
    updateInfo();
    show();
    raise();
    activateWindow();
}

InfoPage::InfoPage(QWidget* parent) : TFrame(parent), m_folder(false)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    //setStyleSheet("background:transparent;");

    setAttribute(Qt::WA_TranslucentBackground);

    setBorder(true);
    setRoundness(15);
    setBorderSize(5);
    setEnableBackground(true);

    initUI();
}

void InfoPage::initUI()
{
    setWindowTitle("File Information");
    setWindowIcon(QIcon(":/icons/OneDrive.svg"));

    iconButton = new QPushButton(this);
    iconButton->setIconSize(QSize(80,80));
    iconButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    closeButton = new HoverButton(this);
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(QSize(45, 45));
    closeButton->setIconSize(QSize(32, 32));
    closeButton->setIcon(QIcon(":/icons/closewhite.svg"));
    closeButton->setStyleSheet(buttonStyle);
    closeButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(2,2,2,2);
    layout->addWidget(closeButton, 0, 0, Qt::AlignRight);
    layout->addWidget(iconButton, 1, 0, 1, 1,Qt::AlignHCenter);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(5);
    //formLayout->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    filePath = new TLabel(this);
    fileSize = new QLabel(this);
    date = new QLabel(this);
    uploadDate = new QLabel(this);

    QString labelStyle = "QLabel{background:transparent;font: 13pt \"Noto Sans\";}";

    filePath->setMinimumHeight(65);
    fileSize->setMinimumHeight(35);
    date->setMinimumHeight(35);
    uploadDate->setMinimumHeight(35);

    filePath->setStyleSheet(labelStyle);
    date->setStyleSheet(labelStyle);
    uploadDate->setStyleSheet(labelStyle);
    fileSize->setStyleSheet(labelStyle);

    formLayout->addRow(filePath);
    formLayout->addRow(fileSize);
    formLayout->addRow(date);
    formLayout->addRow(uploadDate);

    layout->addLayout(formLayout, 2, 0, 1, 1);
    setLayout(layout);

    setFixedSize(400,330);

    connect(closeButton, &QToolButton::clicked, this, &InfoPage::close);
    setStyleSheet("color:white"
                  "QLabel { color : white; }"
                  "QDialog { background:rgba(30,30,30,0.9);border:1px solid lightgreen }"
                  "QPushButton{background:transparent;border:none;}");
}


void InfoPage::updateInfo()
{
    if (m_folder) {
        filePath->setText("Folder Path: " + info.filepath);
        fileSize->setText("Folder Size: " + TCLOUD::formatSize(info.size));
        iconButton->setIcon(QIcon(":/icons/folder.svg"));
    } else {
        filePath->setText("File Path: " + info.filepath);
        fileSize->setText("File Size: " + TCLOUD::formatSize(info.size));
        iconButton->setIcon(QIcon(getIcon()));
    }
    date->setText("Last Edit Date: " + info.lastEditDate.toString());
    uploadDate->setText("Upload Date: " + info.uploadDate.toString());
    update();
}
