#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "tdialog.h"

class NetworkAgent;

namespace Ui {
class InputDialog;
}

class InputDialog : public TDialog
{
    Q_OBJECT
public:
    enum Purpose{
        Rename,
        CreateFolder,
        AuthentificationCode
    };
    static InputDialog* m_dialog;
    static InputDialog* instance();
    static void Init(NetworkAgent* th,QWidget* parent=nullptr);
    static void cleanUp();

    Purpose getPurpose() const;
    void setPurpose(Purpose newPurpose);


    InputDialog& operator<<(const QString& value);


signals:
    void DoneCreatingFolder(const QString&);
    void DoneRenaming(const QString&);
    void canceling();
    void codeReady(const QString&);

private:
    Ui::InputDialog *ui;
    explicit InputDialog(QWidget *parent = nullptr);
    explicit InputDialog(NetworkAgent* NetworkAgent,QWidget *parent = nullptr);
    ~InputDialog();

    Purpose m_purpose;

    QStringList m_data;
    NetworkAgent* m_connect=nullptr;

    void processFolderCreation();
    void processRenaming();
    void cancel();
};

#endif // INPUTDIALOG_H
