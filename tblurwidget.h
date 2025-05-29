#ifndef TBLURWIDGET_H
#define TBLURWIDGET_H

#include "twidget.h"
#include <QGraphicsBlurEffect>
#include <QVBoxLayout>
#include <QWidget>
#include <QPainter>
#include <QLabel>

class TBlurWidget : public TWidget
{
    Q_OBJECT
public:
    explicit TBlurWidget(QWidget *parent = nullptr);
    ~TBlurWidget();

    void setEnableBackground(bool);
    void setBackgroundImage(QString);
    void setBlurRadius(short);

signals:

private:
    TWidget* m_blurredBackground=nullptr;
    QGraphicsBlurEffect* m_blur=nullptr;
    short m_Bradius;

};

#endif // TBLURWIDGET_H
