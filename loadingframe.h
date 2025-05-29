#ifndef LOADINGFRAME_H
#define LOADINGFRAME_H

#include <QObject>
#include <QGridLayout>
#include "tframe.h"
#include "animhandler.h"

class CircularProgressBar;

class LoadingFrame : public TFrame
{
    Q_OBJECT

public:
    explicit LoadingFrame(QWidget *parent = nullptr);
    ~LoadingFrame();
    void start(QRect geometry, const QString &initialText = "Please wait");

    void stop();
    bool isLoading();
    void setMessage(const QString&) const ;


private:
    AnimationHandler* m_handler;
    QGridLayout *m_layout;
    QLabel *m_textLabel;
    CircularProgressBar* m_spinner=nullptr;
    bool loading=false;

};


#endif // LOADINGFRAME_H
