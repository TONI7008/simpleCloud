#ifndef TNOTIFICATION_H
#define TNOTIFICATION_H

#include "popoutframe.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QString>
#include <QLabel>

class TStackedWidget;
class TLabel;
class TCheckMark;


class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    MessageWidget(QWidget* parent=nullptr);
    ~MessageWidget();

    void setMessage(const QString&);
private:
    TLabel* m_label=nullptr;
    QHBoxLayout* m_layout = nullptr;
};


class TNotifaction : public PopoutFrame
{
    Q_OBJECT
public:
    TStackedWidget* stackedWidget() const;
    void setAnimationDuration(int duration);
    void setMaxwidth(int width);
    void setMessage(const QString&,bool error=false);


    static TNotifaction* instance();
    static void cleanUp();
    static TNotifaction* m_dynamicFrame;

    //void setBackgroundImage(const QString&);

signals:
    void incomingNotif();
private:
    explicit TNotifaction(QWidget* parent = nullptr);
    ~TNotifaction();

    TStackedWidget* m_stack = nullptr;
    QHBoxLayout* m_layout = nullptr;
    MessageWidget* m_mWidget=nullptr;

};

#endif // TNOTIFICATION_H
