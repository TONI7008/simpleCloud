#include "hoverbutton.h"
#include <qpropertyanimation.h>
#include "iconcolorizer.h"

QMap<QString, QPair<QIcon, QIcon>> HoverButton::m_iconMap;
QColor HoverButton::m_defaultColor = QColor(71, 158, 245);

HoverButton::HoverButton(QWidget *parent)
    : QPushButton(parent), m_color(Qt::transparent) {
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
    updateIconMap();
    connect(this, &QPushButton::clicked, this, &HoverButton::handleClicked);
}

void HoverButton::setObjectName(const QString& name)
{
    if(m_iconMap.contains(name)){
        setIcon(m_iconMap[name].first);
    }
    QPushButton::setObjectName(name);
}

QColor HoverButton::defaultColor() {
    return m_defaultColor;
}

void HoverButton::setDefaultColor(const QColor &color) {
    if (m_defaultColor != color) {
        m_defaultColor = color;
        updateIconMap();
    }
}

void HoverButton::updateIconMap() {
    m_iconMap.clear();

    // Initialize icon map with colorized icons
    m_iconMap.insert("expandButton",
                     {QIcon(":/icons/expandwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/expandwhite.svg"))});
    m_iconMap.insert("shrinkButton",
                     {QIcon(":/icons/compresswhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/compresswhite.svg"))});

    m_iconMap.insert("addfile",
                     {QIcon(":/icons/addfilewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/addfilewhite.svg"))});
    m_iconMap.insert("addfolder",
                     {QIcon(":/icons/addfolderwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/addfolderwhite.svg"))});
    m_iconMap.insert("prev",
                     {QIcon(":/icons/leftwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/leftwhite.svg"))});
    m_iconMap.insert("next",
                     {QIcon(":/icons/rightwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/rightwhite.svg"))});
    m_iconMap.insert("editUsername",
                     {QIcon(":/icons/editwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/editwhite.svg"))});
    m_iconMap.insert("editEmail",
                     {QIcon(":/icons/editwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/editwhite.svg"))});
    m_iconMap.insert("editPassword",
                     {QIcon(":/icons/editwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/editwhite.svg"))});
    m_iconMap.insert("share",
                     {QIcon(":/icons/sharefilewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/sharefilewhite.svg"))});
    m_iconMap.insert("logout",
                     {QIcon(":/icons/logoutwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/logoutwhite.svg"))});
    m_iconMap.insert("download",
                     {QIcon(":/icons/downloadwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/downloadwhite.svg"))});
    m_iconMap.insert("upload",
                     {QIcon(":/icons/uploadwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/uploadwhite.svg"))});
    m_iconMap.insert("search",
                     {QIcon(":/icons/searchwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/searchwhite.svg"))});
    m_iconMap.insert("refresh",
                     {QIcon(":/icons/refreshwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/refreshwhite.svg"))});
    m_iconMap.insert("playButton",
                     {QIcon(":/icons/resumewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/resumewhite.svg"))});
    m_iconMap.insert("eyeButton",
                     {QIcon(":/icons/eyeclosewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/eyeclosewhite.svg"))});
    m_iconMap.insert("settingButton",
                     {QIcon(":/icons/settingwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/settingwhite.svg"),Qt::black)});
    m_iconMap.insert("appareanceB",
                     {QIcon(":/icons/appareance_white.svg"),
                      IconColorizer::colorize(QIcon(":/icons/appareance_white.svg"),Qt::black)});
    m_iconMap.insert("accountB",
                     {QIcon(":/icons/accountwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/accountwhite.svg"),Qt::black)});
    m_iconMap.insert("utilitiesB",
                     {QIcon(":/icons/toolwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/toolwhite.svg"),Qt::black)});
    m_iconMap.insert("notificationB",
                     {QIcon(":/icons/ringbellwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/ringbellwhite.svg"),Qt::black)});
    m_iconMap.insert("save",
                     {QIcon(":/icons/savewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/savewhite.svg"))});
    m_iconMap.insert("refreshButton",
                     {QIcon(":/icons/refreshwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/refreshwhite.svg"))});
    m_iconMap.insert("closeButton",
                     {QIcon(":/icons/closewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/closewhite.svg"), Qt::red)});
    m_iconMap.insert("closeButton_2",
                     {QIcon(":/icons/closewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/closewhite.svg"), Qt::red)});
    m_iconMap.insert("homeButton",
                     {QIcon(":/icons/homewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/homewhite.svg"), Qt::black)});
    m_iconMap.insert("usageButton",
                     {QIcon(":/icons/storagewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/storagewhite.svg"), Qt::black)});
    m_iconMap.insert("trashButton",
                     {QIcon(":/icons/deletewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/deletewhite.svg"), Qt::black)});
    m_iconMap.insert("addfileButton",
                     {QIcon(":/icons/addfilewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/addfilewhite.svg"), Qt::black)});
    m_iconMap.insert("addfolderButton",
                     {QIcon(":/icons/addfolderwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/addfolderwhite.svg"), Qt::black)});
    m_iconMap.insert("homeButton_2",
                     {QIcon(":/icons/homewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/homewhite.svg"), Qt::black)});
    m_iconMap.insert("usageButton_2",
                     {QIcon(":/icons/storagewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/storagewhite.svg"), Qt::black)});
    m_iconMap.insert("trashButton_2",
                     {QIcon(":/icons/deletewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/deletewhite.svg"), Qt::black)});
    m_iconMap.insert("addfileButton_2",
                     {QIcon(":/icons/addfilewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/addfilewhite.svg"), Qt::black)});
    m_iconMap.insert("addfolderButton_2",
                     {QIcon(":/icons/addfolderwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/addfolderwhite.svg"), Qt::black)});
    m_iconMap.insert("logoutButton",
                     {QIcon(":/icons/logoutwhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/logoutwhite.svg"), Qt::black)});
    m_iconMap.insert("saveButton",
                     {QIcon(":/icons/savewhite.svg"),
                      IconColorizer::colorize(QIcon(":/icons/savewhite.svg"), Qt::black)});
    m_iconMap.insert("RarrowButton",
                     {QIcon(":/icons/right_white.svg"),
                      IconColorizer::colorize(QIcon(":/icons/right_white.svg"), Qt::black)});
    m_iconMap.insert("LarrowButton_2",
                     {QIcon(":/icons/left_white.svg"),
                      IconColorizer::colorize(QIcon(":/icons/left_white.svg"), Qt::black)});
}

void HoverButton::handleClicked() {
    for (auto &name : m_exceptions) {
        if (this->objectName() == name) {
            hoverState();
        }
    }
}

void HoverButton::handleHoverEvent(QHoverEvent* event) {
    if (m_iconMap.contains(this->objectName())) {
        if (event->type() == QEvent::HoverEnter) {
            hoverState();
            emit enterHover();
        } else if (event->type() == QEvent::HoverLeave) {
            leaveState();
            emit leaveHover();
        }
    }
}

void HoverButton::hoverState() {
    if (m_iconMap.contains(this->objectName())) {
        setIcon(m_iconMap.value(this->objectName()).second);
    }
}

void HoverButton::leaveState() {
    if (m_iconMap.contains(this->objectName())) {
        setIcon(m_iconMap.value(this->objectName()).first);
    }
}

void HoverButton::animateColor(const QColor &startValue, const QColor &endValue) {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
