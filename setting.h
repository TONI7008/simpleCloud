#ifndef SETTING_H
#define SETTING_H

#include "twidget.h"
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QSoundEffect>
#include <QEasingCurve>
#include <QPixmap>
#include <QFileDialog>
#include <QDirIterator>
#include <QColorDialog>
#include <QMessageBox>

class SettingsManager;
class TGridLayout;

class CustomToolButton : public QToolButton {
    Q_OBJECT

public:
    explicit CustomToolButton(QWidget *parent = nullptr);
    void setSelected(bool slected);
    bool isSelected();


protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    bool m_hovered;
    bool selected=false;
};




namespace Ui {
class Setting;
}

class Setting : public TWidget
{
    Q_OBJECT

public:
    explicit Setting(SettingsManager* manager,QWidget *parent = nullptr);
    ~Setting();



    void addEasingCurvesToComboBox(QComboBox* comboBox);
    void showNotification();
    void showAppareance();
    void showUtilities();
    void showUserInfo();
    void setupDesign();
    void play();
    void addSongs(const QString &resourcePath, QComboBox *comboBox);

    void manage(CustomToolButton* button);
    void setManager(SettingsManager*);
    bool checkImageWidth(const QString &resourcePath);
    QStringList listLargeImages(const QString &resourceNamespace);
    void setPicture(QPixmap p);
    void loadSettingToUi();
    void addLogic();
    void breakLogic();

    QString convertToPng(const QString &filename);
    void addBackground();
    void showPanel();
    void hidePanel();

signals:
    void aboutToClose();
    void backgroundChanged(const QString&);
    void startLoading(const QString&);
    void stopLoading();
    void imageReady(QString);


private:
    Ui::Setting *ui;
    QSoundEffect *effect=nullptr;
    QPixmap backgroundImage=QPixmap(":/Icons/lines.jpg");
    short int spacing;
    const short int buttonH=45;
    void Close();
    void build();
    void Init();

    TGridLayout* labelLayout=nullptr;
    QStringList backgroundList;
    SettingsManager* m_Manager=nullptr;
    short transparency=250;
    QColor m_color;

    short index=0;
    const short smallestWidth=840;

    void loadNextBackgroundImage(int index);
    void addSongRecursive(const QStringList &files, QComboBox *comboBox, int index);


    QList<QMetaObject::Connection> m_connections;
};

inline QMap<QEasingCurve::Type, QString> easingTypes = {
    {QEasingCurve::Linear, "Linear"},
    {QEasingCurve::InQuad, "InQuad"},
    {QEasingCurve::OutQuad, "OutQuad"},
    {QEasingCurve::InOutQuad, "InOutQuad"},
    {QEasingCurve::InCubic, "InCubic"},
    {QEasingCurve::OutCubic, "OutCubic"},
    {QEasingCurve::InOutCubic, "InOutCubic"},
    {QEasingCurve::InQuart, "InQuart"},
    {QEasingCurve::OutQuart, "OutQuart"},
    {QEasingCurve::InOutQuart, "InOutQuart"},
    {QEasingCurve::InQuint, "InQuint"},
    {QEasingCurve::OutQuint, "OutQuint"},
    {QEasingCurve::InOutQuint, "InOutQuint"},
    {QEasingCurve::InSine, "InSine"},
    {QEasingCurve::OutSine, "OutSine"},
    {QEasingCurve::InOutSine, "InOutSine"},
    {QEasingCurve::InExpo, "InExpo"},
    {QEasingCurve::OutExpo, "OutExpo"},
    {QEasingCurve::InOutExpo, "InOutExpo"},
    {QEasingCurve::InCirc, "InCirc"},
    {QEasingCurve::OutCirc, "OutCirc"},
    {QEasingCurve::InOutCirc, "InOutCirc"},
    {QEasingCurve::InElastic, "InElastic"},
    {QEasingCurve::OutElastic, "OutElastic"},
    {QEasingCurve::InOutElastic, "InOutElastic"},
    {QEasingCurve::InBack, "InBack"},
    {QEasingCurve::OutBack, "OutBack"},
    {QEasingCurve::InOutBack, "InOutBack"},
    {QEasingCurve::InBounce, "InBounce"},
    {QEasingCurve::OutBounce, "OutBounce"},
    {QEasingCurve::InOutBounce, "InOutBounce"},
    {QEasingCurve::CosineCurve, "CosineCurve"},
    {QEasingCurve::SineCurve, "SineCurve"},};

#endif // SETTING_H
