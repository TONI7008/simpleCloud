#ifndef BACKGROUNDFRAME_H
#define BACKGROUNDFRAME_H

#include "tframe.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPainter>
#include <QPixmap>
#include <QToolButton>

class BackgroundFrame : public TFrame{
    Q_OBJECT
public:


    BackgroundFrame(QWidget* parent=nullptr);
    ~BackgroundFrame();
    void setSelected(bool s);
    bool isSelected() {return selected;}
    void manage();
    void setText(QString text);
    void SetSimpleType(bool);

    void setPixmap(QPixmap);
    QString text();

    static QList<BackgroundFrame*> m_list;

signals:
    void kill();

private:
    bool selected=false;
    QString m_text;
    bool m_simpleType=false;

    QToolButton* deleteButton;
    QPixmap m_backgroundImage;

    QString defaultStyle=R"(
    QToolButton{border-radius:5px;background:rgba(30,30,30,0.4);}
    QToolButton:hover{border-radius:5px;background:rgba(30,30,30,0.7);}
    )";
};

#endif // BACKGROUNDFRAME_H
