#ifndef SETTINGMANAGER_H
#define SETTINGMANAGER_H

#include "tcloud.h"
#include <QJsonObject>
#include <QPixmap>
#include <QEasingCurve>


class mainThread;

class SettingsManager : public QObject {
    Q_OBJECT
public:
    SettingsManager(mainThread*,const QString &username, QObject *parent = nullptr);

    void displaySettings() const;
    bool save();
    QString debug();
    void reset();
    QString getUsername();
    QString getEmail();
    bool edited() const;
    void setEdited(bool);


    // Getters
    TCLOUD::Theme theme();
    short int animationDuration();
    QString backgroundColor();
    QString backgroundImage();
    bool customColor();
    bool customImage();
    bool transparency();
    QEasingCurve easingCurve();
    short int fontSize();
    QFont fontFamily();
    bool animationEnabled();

    bool notificationPageEnable();
    QString notificationSon();
    bool poppoutNotificationPage();
    bool useSystemNotificationPage();
    bool enableNotification();
    double notificationVolume();
    short notificationDuration();

    bool changeUserDataPath();
    QPixmap defaultProfilePicture();
    QString userDataPath();

    bool synchronize();
    bool launchOnStartUp();
    bool startDownloadStartup();
    bool enableTrash();
    short int deleteFileAfter();
    bool deleteAfterUpload();
    short uploadLimit();
    short downloadLimit();
    short uploadSpeedLimit();
    short downloadSpeedLimit();
    short int trashMaxSize();
    bool deleteGreaterThan5();


    // Setters
    void setTheme(TCLOUD::Theme theme);
    void setAnimationDuration(short int duration);
    void setBackgroundColor(const QString &color);
    void setBackgroundImage(const QString &image);
    void setCustomColor(bool custom);
    void setCustomImage(bool custom);
    void setEasingCurve(QEasingCurve curve);
    void setFontSize(short int size);
    void setFontFamily(const QString &family);
    void setTransparency(bool);

    void setNotificationPageEnable(bool able);
    void setNotificationSon(const QString &son);
    void setPoppoutNotificationPage(bool poppout);
    void setUseSystemNotificationPage(bool useSystem);
    void setEnableNotification(bool);
    void setNotificationDuration(short);
    void setNotificationVolume(double);

    void setChangeUserDataPath(bool change);
    void setDefaultProfilePicture(QPixmap);
    void setUserDataPath(const QString &path);

    void setSynchronize(bool sync);
    void setLaunchOnStartUp(bool launch);
    void setStartDownloadStartup(bool start);
    void setEnableTrash(bool disable);
    void setDeleteFileAfter(short int days);
    void setDeleteAfterUpload(bool deleteAfter);
    void setUploadLimit(short int limit);
    void setSpeedUploadLimit(short limit);
    void setDownloadLimit(short int limit);
    void setSpeedDownloadLimit(short limit);
    void setTrashMaxSize(short int size);
    void mergeWithDefaultSettings();
    void setAnimationEnabled(bool);
    void setDeleteGreaterThan5(bool booleen);

    //for the class

    void setUsername(const QString &newUsername);
    bool loadResource();
    bool loadFromRawData(const QByteArray &rawData);


    void init();
    void updateProfilePicture(QByteArray);
signals:
    void themeChanged(TCLOUD::Theme);
    void animationDurationChanged(int);
    void backgroundColorChanged(const QString);
    void backgroundImageChanged(const QString);
    void customColorChanged(bool);
    void customImageChanged(bool);
    void easingCurveChanged();
    void fontSizeChanged(short);
    void fontFamilyChanged(QString);
    void animationEnabledChanged(bool);
    void transparencyChanged(bool);

    void notificationPageEnableChanged(bool);
    void notificationSonChanged();
    void poppoutNotificationPageChanged();
    void useSystemNotificationPageChanged();
    void enableNotificationChanged();
    void notificationDurationChanged(short);
    void notificationVolumeChanged(double);

    void changeUserDataPathChanged();
    void defaultProfilePictureChanged(QPixmap);
    void userDataPathChanged();

    void synchronizeChanged();
    void launchOnStartUpChanged();
    void startDownloadStartupChanged();
    void enableTrashChanged(bool);
    void deleteFileAfterChanged();
    void deleteAfterUploadChanged();
    void uploadLimitChanged(short);
    void downloadLimitChanged(short);
    void uploadSpeedLimitChanged(short);
    void downloadSpeedLimitChanged(short);
    void trashMaxSizeChanged();
    void deleteGreaterThan5Changed(bool);

private:
    QString filePath;
    QString username;
    QJsonObject rootObject;

    QJsonObject TCloudSettings;
    QJsonObject defaultSettings;

    QJsonObject userSettings;

    QJsonObject appareanceSettings;
    QJsonObject utilitySettings;
    QJsonObject notificationSettings;
    QJsonObject userSectionSettings;
    mainThread* m_thread=nullptr;

    QByteArray defaultPicture;

    bool loaded=false;
    bool synchro=false;
    bool wasEdited=false;

    bool isConnected();
    void updateUserSetting();
    void copyDefaultSettingsToUser();
    bool createDir(const QString&);
    void updateSettings();
    bool stop=false;
};

#endif // SETTINGMANAGER_H
