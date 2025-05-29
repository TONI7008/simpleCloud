#include "backgroundframe.h"
#include <QGridLayout>

QList<BackgroundFrame*> BackgroundFrame::m_list;

BackgroundFrame::BackgroundFrame(QWidget *parent)
    : TFrame{parent}
{
    //setScaledContents(true);

    deleteButton=new QToolButton(this);
    deleteButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    deleteButton->setIcon(QIcon(":/icons/deletewhite.svg"));
    deleteButton->setIconSize(QSize(24,24));
    deleteButton->setFixedSize(30,30);

    connect(this,&BackgroundFrame::Clicked,this,[this]{
        if(!m_simpleType){
            manage();
        }
    });

    if(m_simpleType){
        setEnableBackground(false);
        setEnableBackground(false);
    }else{
        m_list.append(this);
        setStyleSheet(defaultStyle);
        //setEnabledBorder(true);
        setBorderSize(4);
    }

    QGridLayout* layout=new QGridLayout(this);
    layout->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(deleteButton);
    setLayout(layout);


    connect(deleteButton,&QToolButton::clicked,this,[this]{
        m_list.removeOne(this);
        emit kill();
    });


    setClickInterval(50);
}

BackgroundFrame::~BackgroundFrame()
{

}

void BackgroundFrame::setSelected(bool s)
{
    selected=s;
    if(selected){
        setEnabledBorder(selected);
    }else{
        setEnabledBorder(selected);
    }

}

void BackgroundFrame::manage()
{
    if(m_list.isEmpty()) return;
    setSelected(true);
    for(auto &b : m_list){
        if(b==this){
            continue;
        }
        b->setSelected(false);
    }
}


void BackgroundFrame::setText(QString text)
{
    m_text=text;
}

void BackgroundFrame::SetSimpleType(bool s)
{
    m_simpleType=s;
    if(m_list.contains(this)){
        m_list.remove(m_list.indexOf(this));
    }
}

void BackgroundFrame::setPixmap(QPixmap f)
{
    setBackgroundImage(f);
}

QString BackgroundFrame::text()
{
    return m_text;
}

