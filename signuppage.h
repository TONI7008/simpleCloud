#ifndef SIGNUPPAGE_H
#define SIGNUPPAGE_H

#include <QWidget>
#include <QMessageBox>

#include "twidget.h"

class mainThread;
class TStackedWidget;


namespace Ui {
class SignUpPage;
}

class SignUpPage : public TWidget
{
    Q_OBJECT

public:
    explicit SignUpPage(mainThread* con,TStackedWidget* stack,QWidget *parent = nullptr);
    ~SignUpPage();

    void start(const QString&);
    void stop();

signals:
    void startLoading(const QString&);
    void stopLoading();

private:
    Ui::SignUpPage *ui;
    mainThread* m_thread=nullptr;
    TStackedWidget* m_stack=nullptr;

    void signUp();
};

#endif // SIGNUPPAGE_H
