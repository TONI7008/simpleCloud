#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include "twidget.h"


class mainThread;
class TStackedWidget;

namespace Ui {
class loginPage;
}

class loginPage : public TWidget
{
    Q_OBJECT

public:
    explicit loginPage(mainThread* con,TStackedWidget* stack,QWidget *parent = nullptr);
    ~loginPage();

    QString username();
    void start(const QString&) const;
    void stop();

signals:
    void startLoading(const QString&);
    void stopLoading();
    void Done();

private:
    Ui::loginPage *ui;

    mainThread* m_thread;
    TStackedWidget* m_stack;
    void showMessage(QString msg);
    short _pos;
    void login();
    void clean();
};

#endif // LOGINPAGE_H
