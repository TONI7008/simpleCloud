#include "settingmanager.h"
#include "mainthread.h"

#include <QImage>
#include <QJsonDocument>
#include <QDir>
#include <QFont>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QElapsedTimer>

SettingsManager::SettingsManager(mainThread* th,const QString &username, QObject *parent)
    : QObject(parent), username(username),m_thread(th) {
    QElapsedTimer timer;
    timer.start();
    init();
    qDebug() << "elapse time setting manager :"<< timer.elapsed();

}
void SettingsManager::init(){
    QElapsedTimer timer;
    timer.start();
    createDir(".");
    qDebug() << "elapse time create dir :"<< timer.elapsed();

    if (isConnected()) {
        if (!loadFromRawData(m_thread->getUserSetting())) {
            //QMessageBox::critical(nullptr, "Error", "Failed to load preferences");
            loadResource();
        }
    }
    filePath = userDataPath() + QDir::separator() + "settings.json";
}

bool SettingsManager::loadFromRawData(const QByteArray &rawData) {

    QJsonDocument document = QJsonDocument::fromJson(rawData);
    if (!document.isObject()) {
        qWarning() << "Failed to parse JSON from raw data.";
        return false;
    }

    rootObject = document.object();
    TCloudSettings = rootObject.value("TCloud").toObject();
    defaultSettings = rootObject.value("default_setting").toObject();

    if (rootObject.contains(username)) {
        userSettings = rootObject.value(username).toObject();
    } else {
        copyDefaultSettingsToUser();  // Copier les paramètres par défaut dans userSettings
    }

    mergeWithDefaultSettings();
    updateSettings();
    return true;
}

bool SettingsManager::loadResource() {
    QFile file(":/Resources/settings.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file.fileName();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(data);
    if (document.isNull() || !document.isObject()) {
        qWarning() << "Failed to parse JSON: " << file.fileName();
        return false;
    }

    rootObject = document.object();
    TCloudSettings = rootObject.value("TCloud").toObject();
    defaultSettings = rootObject.value("default_setting").toObject();

    if (rootObject.contains(username)) {
        userSettings = rootObject.value(username).toObject();
    } else {
        copyDefaultSettingsToUser();
    }

    QTimer::singleShot(550, this, &SettingsManager::mergeWithDefaultSettings);
    updateSettings();

    return true;
}


void SettingsManager::updateProfilePicture(QByteArray pic)
{
    defaultPicture=pic;
    QImage image = QImage::fromData(pic);
    emit defaultProfilePictureChanged(QPixmap::fromImage(image));
}

bool SettingsManager::save() {

    QJsonObject saveObject = rootObject;
    saveObject.remove("default_setting");

    if (synchro) {
        QJsonDocument document(saveObject);
        QByteArray data = document.toJson(QJsonDocument::Indented);
        //if(QImage::fromData(defaultPicture).save("essai.png")) qDebug() << "works for the client";
        m_thread->synchronize(data,defaultPicture);
        return true;
    } else {
        QFile file(filePath);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Échec de l'ouverture du fichier pour écriture:" << filePath;
            return false;
        }

        QJsonDocument document(saveObject);
        file.write(document.toJson(QJsonDocument::Indented));
        file.close();
        wasEdited=false;
        return true;
    }
}

void SettingsManager::updateUserSetting() {
    userSettings["appareance"] = appareanceSettings;
    userSettings["Utilities"] = utilitySettings;
    userSettings["notification"] = notificationSettings;
    userSettings["userSection"] = userSectionSettings;

    rootObject[username] = userSettings;
    TCloudSettings["setting_to_use"] = username;
    rootObject["TCloud"] = TCloudSettings;
}

void SettingsManager::copyDefaultSettingsToUser() {
    userSettings = defaultSettings;
    updateUserSetting();
}

void SettingsManager::updateSettings() {
    appareanceSettings = userSettings.value("appareance").toObject();
    utilitySettings = userSettings.value("Utilities").toObject();
    notificationSettings = userSettings.value("notification").toObject();
    userSectionSettings = userSettings.value("userSection").toObject();
}

QString SettingsManager::debug() {
    QStringList result;

    result.append("TCloudSettings: " + QJsonDocument(TCloudSettings).toJson(QJsonDocument::Indented));
    result.append(username + " Settings: " + QJsonDocument(userSettings).toJson(QJsonDocument::Indented));

    return result.join("\n\n");
}


void SettingsManager::reset() {

    userSettings = defaultSettings;
    rootObject[username] = userSettings;
    updateSettings();
    wasEdited=false;
}

QString SettingsManager::getUsername()
{
    return m_thread->getUsername();
}

QString SettingsManager::getEmail()
{
    return "userEmail";
}

bool SettingsManager::edited() const
{
    return wasEdited;
}

void SettingsManager::setEdited(bool ed)
{
    wasEdited=ed;
}


bool SettingsManager::createDir(const QString &filePath)
{
    QString dirPath=filePath;
    QDir dir(dirPath);
    return dir.mkdir("TCloudSettings");
}

void SettingsManager::displaySettings() const {
    qDebug() << "TCloud Settings:" << TCloudSettings;qDebug();
    qDebug() << "Default Settings:" << defaultSettings;qDebug();
    qDebug() << "User Settings for" << username << ":" << userSettings;qDebug();
}

#include <QHostInfo>

bool SettingsManager::isConnected() {
    // This checks if we can resolve a domain name (much faster than HTTP request)
    QHostInfo hostInfo = QHostInfo::fromName("google.com");
    bool connected = hostInfo.error() == QHostInfo::NoError;
    synchro = connected;
    return connected;
}

void SettingsManager::mergeWithDefaultSettings() {
    Q_FOREACH (const QString &key, defaultSettings.keys()) {
        if (!userSettings.contains(key)) {
            userSettings[key] = defaultSettings[key];
        } else if (userSettings[key].isObject()) {
            QJsonObject defaultSubObject = defaultSettings[key].toObject();
            QJsonObject userSubObject = userSettings[key].toObject();

            Q_FOREACH (const QString &subKey, defaultSubObject.keys()) {
                if (!userSubObject.contains(subKey)) {
                    userSubObject[subKey] = defaultSubObject[subKey];
                }
            }

            userSettings[key] = userSubObject;
        }
    }
    rootObject[username] = userSettings;
}

void SettingsManager::setUsername(const QString &newUsername)
{
    if (newUsername == username) {
        qDebug() << "New username is the same as the current one. No changes made.";
        return;
    }

    if (rootObject.contains(username)) {
        QJsonObject currentUserSettings = rootObject.value(username).toObject();
        rootObject.insert(newUsername, currentUserSettings);
    } else {
        copyDefaultSettingsToUser();
    }

    QString oldUsername = username;
    username = newUsername;

    userSettings = rootObject.value(newUsername).toObject();
    updateSettings();

    TCloudSettings["setting_to_use"] = newUsername;
    rootObject["TCloud"] = TCloudSettings;

    rootObject.remove(oldUsername);

    save();
}

// Getters
TCLOUD::Theme SettingsManager::theme() {
    QString themeStr = appareanceSettings.value("theme").toString();
    return (themeStr == "Dark") ? TCLOUD::Dark : TCLOUD::Light;
}

short int SettingsManager::animationDuration() {
    return appareanceSettings.value("animation_duration").toInt();
}

QString SettingsManager::backgroundColor() {
    return appareanceSettings.value("backgroundColor").toString();
}

QString SettingsManager::backgroundImage() {
    return appareanceSettings.value("backgroundImage").toString();
}

bool SettingsManager::customColor() {
    return appareanceSettings.value("custom_background_color").toBool();
}

bool SettingsManager::customImage() {
    return appareanceSettings.value("custom_background_image").toBool();
}

bool SettingsManager::transparency()
{
    return appareanceSettings.value("transparency").toBool();
}

QEasingCurve SettingsManager::easingCurve() {
    QString curveStr = appareanceSettings.value("easing_curve").toString();
    if (curveStr == "QEasing::OutCirc") return QEasingCurve::OutCirc;
    return QEasingCurve::Linear; // Default
}

short int SettingsManager::fontSize() {
    return appareanceSettings.value("font_size").toInt();
}

QFont SettingsManager::fontFamily() {
    QFont font;
    font.fromString(appareanceSettings.value("font_style").toString());
    return font;
}

bool SettingsManager::animationEnabled()
{
    return appareanceSettings.value("enable_animation").toBool();
}

bool SettingsManager::notificationPageEnable() {
    return notificationSettings.value("enable_notification_page").toBool();
}

QString SettingsManager::notificationSon() {
    return notificationSettings.value("notification_son").toString();
}

bool SettingsManager::poppoutNotificationPage() {
    return notificationSettings.value("popout_notification_page").toBool();
}

bool SettingsManager::useSystemNotificationPage() {
    return notificationSettings.value("use_system_notification_page").toBool();
}

bool SettingsManager::enableNotification()
{
    return notificationSettings.value("enable_notification").toBool();
}

double SettingsManager::notificationVolume()
{
    return notificationSettings.value("notification_volume").toDouble();
}

short SettingsManager::notificationDuration()
{
    return notificationSettings.value("notification_duration").toInt();
}

bool SettingsManager::changeUserDataPath() {
    return userSectionSettings.value("change_user_data_path").toBool();
}

QPixmap SettingsManager::defaultProfilePicture() {
    QImage image = QImage::fromData(defaultPicture);
    return QPixmap::fromImage(image);
}

QString SettingsManager::userDataPath() {
    return userSectionSettings.value("user_data_path").toString();
}

bool SettingsManager::synchronize() {
    return utilitySettings.value("synchronize_preferences").toBool();
}

bool SettingsManager::launchOnStartUp() {
    return utilitySettings.value("launch_on_startup").toBool();
}

bool SettingsManager::startDownloadStartup() {
    return utilitySettings.value("start_lazy_download_on_startup").toBool();
}

bool SettingsManager::enableTrash() {
    return utilitySettings.value("enable_trash").toBool();
}

short int SettingsManager::deleteFileAfter() {
    return utilitySettings.value("delete_deleted_file_after").toInt();
}

bool SettingsManager::deleteAfterUpload() {
    return utilitySettings.value("delete_after_upload").toBool();
}

short SettingsManager::uploadLimit() {
    return utilitySettings.value("upload_limit").toInt();
}

short SettingsManager::downloadLimit() {
    return utilitySettings.value("download_limit").toInt();
}
short SettingsManager::uploadSpeedLimit() {
    return utilitySettings.value("upload_speed_limit").toInt();
}

short SettingsManager::downloadSpeedLimit() {
    return utilitySettings.value("download_speed_limit").toInt();
}

short int SettingsManager::trashMaxSize() {
    return utilitySettings.value("recycle_bin_max_size").toInt();
}

bool SettingsManager::deleteGreaterThan5()
{
    return utilitySettings.value("automatically_delete_file_greater_than_5GB").toBool();
}

// Setters
void SettingsManager::setTheme(TCLOUD::Theme theme) {
    appareanceSettings["theme"] = (theme == TCLOUD::Dark) ? "Dark" : "Light";
    emit themeChanged(theme);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setAnimationDuration(short int duration) {
    appareanceSettings["animation_duration"] = duration;
    emit animationDurationChanged(duration);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setBackgroundColor(const QString &color) {
    appareanceSettings["backgroundColor"] = color;
    emit backgroundColorChanged(color);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setBackgroundImage(const QString &image) {
    appareanceSettings["backgroundImage"] = image;
    emit backgroundImageChanged(image);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setCustomColor(bool custom) {
    appareanceSettings["custom_background_color"] = custom;
    emit customColorChanged(custom);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setCustomImage(bool custom) {
    appareanceSettings["custom_background_image"] = custom;
    emit customImageChanged(custom);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setEasingCurve(QEasingCurve curve) {
    QString curveStr;
    switch (curve.type()) {
    case QEasingCurve::Linear:
        curveStr = "QEasingCurve::Linear";
        break;
    case QEasingCurve::InQuad:
        curveStr = "QEasingCurve::InQuad";
        break;
    case QEasingCurve::OutQuad:
        curveStr = "QEasingCurve::OutQuad";
        break;
    case QEasingCurve::InOutQuad:
        curveStr = "QEasingCurve::InOutQuad";
        break;
    // Ajouter d'autres cas pour les différentes courbes si nécessaire
    default:
        curveStr = "QEasingCurve::Linear";
        break;
    }
    appareanceSettings["easing_curve"] = curveStr;
    emit easingCurveChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setFontSize(short int size) {
    appareanceSettings["font_size"] = size;
    emit fontSizeChanged(size);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setFontFamily(const QString &family) {
    appareanceSettings["font_style"] = family;
    emit fontFamilyChanged(family);
    updateUserSetting();

    wasEdited=true;

}

void SettingsManager::setTransparency(bool t)
{
    appareanceSettings["transparency"]=t;
    emit transparencyChanged(t);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setNotificationPageEnable(bool able) {
    notificationSettings["enable_notification_page"] = able;
    emit notificationPageEnableChanged(able);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setNotificationSon(const QString &son) {
    notificationSettings["notification_son"] = son;
    emit notificationSonChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setPoppoutNotificationPage(bool poppout) {
    notificationSettings["popout_notification_page"] = poppout;
    emit poppoutNotificationPageChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setUseSystemNotificationPage(bool useSystem) {
    notificationSettings["use_system_notification_page"] = useSystem;
    emit useSystemNotificationPageChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setEnableNotification(bool b)
{
    notificationSettings["enable_notification"] = b;
    emit enableNotificationChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setNotificationDuration(short d)
{
    notificationSettings["notification_duration"]=d;
    emit notificationDurationChanged(d);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setNotificationVolume(double v)
{
    notificationSettings["notification_volume"]=v;
    emit notificationVolumeChanged(v);
    updateUserSetting();

    wasEdited=true;

}

void SettingsManager::setChangeUserDataPath(bool change) {
    userSectionSettings["change_user_data_path"] = change;
    emit changeUserDataPathChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setDefaultProfilePicture(QPixmap picture) {
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    picture.toImage().save(&buffer, "PNG");
    defaultPicture = byteArray;
    emit defaultProfilePictureChanged(picture);

    wasEdited=true;
}


void SettingsManager::setUserDataPath(const QString &path) {
    userSectionSettings["user_data_path"] = path;
    filePath=path+QDir::separator()+"settings.json";
    emit userDataPathChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setSynchronize(bool sync) {
    utilitySettings["synchronize_preferences"] = sync;
    emit synchronizeChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setLaunchOnStartUp(bool launch) {
    utilitySettings["launch_on_startup"] = launch;
    emit launchOnStartUpChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setStartDownloadStartup(bool start) {
    utilitySettings["start_lazy_download_on_startup"] = start;
    emit startDownloadStartupChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setEnableTrash(bool able) {
    utilitySettings["enable_trash"] = able;
    emit enableTrashChanged(able);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setDeleteFileAfter(short int days) {
    utilitySettings["delete_deleted_file_after"] = days;
    emit deleteFileAfterChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setDeleteAfterUpload(bool deleteAfter) {
    utilitySettings["delete_after_upload"] = deleteAfter;
    emit deleteAfterUploadChanged();
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setSpeedUploadLimit(short int limit) {
    utilitySettings["upload_speed_limit"] = limit;
    emit uploadSpeedLimitChanged(limit);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setSpeedDownloadLimit(short int limit) {
    utilitySettings["download_speed_limit"] = limit;
    emit downloadSpeedLimitChanged(limit);
    updateUserSetting();

    wasEdited=true;
}
void SettingsManager::setUploadLimit(short int limit) {
    utilitySettings["upload_limit"] = limit;
    emit uploadLimitChanged(limit);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setDownloadLimit(short int limit) {
    utilitySettings["download_limit"] = limit;
    emit downloadLimitChanged(limit);
    updateUserSetting();

    wasEdited=true;
}
void SettingsManager::setDeleteGreaterThan5(bool booleen) {
    utilitySettings["automatically_delete_file_greater_than_5GB"] = booleen;
    emit deleteGreaterThan5Changed(booleen);
    updateUserSetting();

    wasEdited=true;
}

void SettingsManager::setAnimationEnabled(bool g)
{
    appareanceSettings["enable_animation"]= g;
    emit animationEnabledChanged(g);
    updateUserSetting();

    wasEdited=true;
}


void SettingsManager::setTrashMaxSize(short int size) {
    utilitySettings["recycle_bin_max_size"] = size;
    emit trashMaxSizeChanged();
    updateUserSetting();

    wasEdited=true;
}

// Helper functions
