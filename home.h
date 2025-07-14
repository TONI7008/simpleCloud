#ifndef HOME_H
#define HOME_H

#include "twidget.h"
#include <QMainWindow>
#include <QTableView>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QDir>
#include <QDateTime>
#include <QHeaderView>

class NetworkAgent;
class TSecurityManager;
class TFileManager;
class LoadingFrame;
class ThreadManager;
class SettingsManager;
class Setting;
class InputDialog;
class InfoPage;
class loginPage;
class SignUpPage;
class DynamicFrameAssistant;
class eltCore;
class HoverButton;
class TFlowLayout;

QT_BEGIN_NAMESPACE
namespace Ui {
class Home;
}
QT_END_NAMESPACE



class Home : public TWidget
{
    Q_OBJECT

public:
    Home(TSecurityManager* smanager,QWidget *parent = nullptr);
    ~Home();

    void load(const QString&);
    void unLoad();
    void refresh();
    void setBackgroundImage(const QString&);
    void setCurrentWidget(QWidget*);
    void pop(bool show);
    void _pop(bool show);
    void makeTransparent(bool);


    void initView(QTableView *tableView,QList<eltCore> files);

signals:
    void startLoading(const QString&);
    void stopLoading();

private:
    Ui::Home *ui;
    ThreadManager* m_tManager=nullptr;
    NetworkAgent* m_connect=nullptr;
    TFileManager* m_fileManager=nullptr;
    LoadingFrame* m_loader=nullptr;
    Setting* m_setting=nullptr;
    SettingsManager* m_settingManager=nullptr;
    loginPage* m_loginPage=nullptr;
    SignUpPage* m_signUpPage=nullptr;
    DynamicFrameAssistant* m_assistant=nullptr;
    DynamicFrameAssistant* m_assistant_2=nullptr;
    TFlowLayout* m_layout1=nullptr;

    bool wasRefresh=false;


    //copy static member
    InputDialog* m_iDialog=nullptr;
    InfoPage* m_infoPage=nullptr;

    void handleResize();

    void handleHide();
    void handleShow();
};
#endif // HOME_H
