#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QWidget>

class mainThread;

namespace Ui {
class InputDialog;
}

class InputDialog : public QWidget
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
    static void Init(mainThread* th,QWidget* parent=nullptr);
    static void cleanUp();

    Purpose getPurpose() const;
    void setPurpose(Purpose newPurpose);


    InputDialog& operator<<(const QString& value);


signals:
    void DoneCreatingFolder(const QString&);
    void DoneRenaming(const QString&);
    void canceling();
private:
    Ui::InputDialog *ui;
    explicit InputDialog(QWidget *parent = nullptr);
    explicit InputDialog(mainThread* mainthread,QWidget *parent = nullptr);
    ~InputDialog();

    Purpose m_purpose;

    QStringList m_data;
    mainThread* m_connect=nullptr;

    void processFolderCreation();
    void processRenaming();
    void cancel();
};

#endif // INPUTDIALOG_H
