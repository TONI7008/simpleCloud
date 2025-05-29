#include "tblurwidget.h"

TBlurWidget::TBlurWidget(QWidget *parent)
    : TWidget{parent},m_Bradius(0)
{
    m_blurredBackground = new TWidget(this);
    TWidget::setEnableBackground(false);

    m_blurredBackground->setEnableBackground(false);
    m_blurredBackground->lower(); // Ensure it's behind children

    // Apply blur effect only to the background
    m_blur = new QGraphicsBlurEffect(this);
    m_blur->setBlurHints(QGraphicsBlurEffect::QualityHint);
    m_blur->setBlurRadius(m_Bradius);
    m_blurredBackground->setGraphicsEffect(m_blur);

    connect(this,&TWidget::resizing,this,[this]{
        m_blurredBackground->setGeometry(this->rect());
    });
}

TBlurWidget::~TBlurWidget()
{

}

void TBlurWidget::setEnableBackground(bool en)
{
    m_blurredBackground->setEnableBackground(en);
}

void TBlurWidget::setBackgroundImage(QString str)
{

    m_blurredBackground->setBackgroundImage(str);
}

void TBlurWidget::setBlurRadius(short r)
{
    m_Bradius=r;
    m_blur->setBlurRadius(m_Bradius);
}
