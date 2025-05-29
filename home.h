#ifndef HOME_H
#define HOME_H

#include "twidget.h"
#include "tcloud.h" // For TCLOUD::Theme

class mainThread;
class TSecurityManager;
class TFileManager;
class LoadingFrame;
class ThreadManager;
class SettingsManager;
class Setting;
class InputDialog;
class InfoPage;

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

signals:
    void startLoading(const QString&);
    void stopLoading();

private slots:
    void applyAppTheme(TCLOUD::Theme theme);

private:
    Ui::Home *ui;
    ThreadManager* m_tManager=nullptr;
    mainThread* m_connect=nullptr;
    TFileManager* m_fileManager=nullptr;
    LoadingFrame* m_loader=nullptr;
    Setting* m_setting=nullptr;
    SettingsManager* m_settingManager=nullptr;


    //copy static member
    InputDialog* m_iDialog=nullptr;
    InfoPage* m_infoPage=nullptr;

    void handleResize();

};
#endif // HOME_H
