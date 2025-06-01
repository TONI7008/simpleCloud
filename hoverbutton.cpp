#include "hoverbutton.h"
#include <qpropertyanimation.h>

HoverButton::HoverButton(QWidget *parent) : QPushButton(parent), m_color(Qt::transparent) {
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
    m_state.setNormal();
    //setIconSize(m_globalIconSize);

    // Initialize icon map with simplified structure
    m_iconMap.insert("addfile", {QIcon(":/icons/addfilewhite.svg"), QIcon(":/icons/addfileblue.svg")});
    m_iconMap.insert("addfolder", {QIcon(":/icons/addfolderwhite.svg"), QIcon(":/icons/addfolderblue.svg")});
    m_iconMap.insert("prev", {QIcon(":/icons/leftwhite.svg"), QIcon(":/icons/leftblue.svg")});
    m_iconMap.insert("next", {QIcon(":/icons/rightwhite.svg"), QIcon(":/icons/rightblue.svg")});
    m_iconMap.insert("editUsername", {QIcon(":/icons/editwhite.svg"), QIcon(":/icons/editblue.svg")});
    m_iconMap.insert("editEmail", {QIcon(":/icons/editwhite.svg"), QIcon(":/icons/editblue.svg")});
    m_iconMap.insert("editPassword", {QIcon(":/icons/editwhite.svg"), QIcon(":/icons/editblue.svg")});
    m_iconMap.insert("share", {QIcon(":/icons/sharefilewhite.svg"), QIcon(":/icons/sharefileblue.svg")});
    m_iconMap.insert("logout", {QIcon(":/icons/logoutwhite.svg"), QIcon(":/icons/logoutblue.svg")});
    m_iconMap.insert("download", {QIcon(":/icons/downloadwhite.svg"), QIcon(":/icons/downloadblue.svg")});
    m_iconMap.insert("upload", {QIcon(":/icons/uploadwhite.svg"), QIcon(":/icons/uploadblue.svg")});
    m_iconMap.insert("search", {QIcon(":/icons/searchwhite.svg"), QIcon(":/icons/searchblue.svg")});
    m_iconMap.insert("refresh", {QIcon(":/icons/refreshwhite.svg"), QIcon(":/icons/refreshblue.svg")});
    m_iconMap.insert("playButton", {QIcon(":/icons/resumewhite.svg"), QIcon(":/icons/resumeblue.svg")});
    m_iconMap.insert("eyeButton", {QIcon(":/icons/eyeclosewhite.svg"), QIcon(":/icons/eyecloseblue.svg")});
    m_iconMap.insert("settingButton", {QIcon(":/icons/settingwhite.svg"), QIcon(":/icons/settingblue.svg")});
    m_iconMap.insert("appareanceB", {QIcon(":/icons/appareance_white.svg"), QIcon(":/icons/appareance_blue.svg")});
    m_iconMap.insert("accountB", {QIcon(":/icons/accountwhite.svg"), QIcon(":/icons/accountblue.svg")});
    m_iconMap.insert("utilitiesB", {QIcon(":/icons/toolwhite.svg"), QIcon(":/icons/toolblue.svg")});
    m_iconMap.insert("notificationB", {QIcon(":/icons/ringbellwhite.svg"), QIcon(":/icons/ringbellblue.svg")});
    m_iconMap.insert("save", {QIcon(":/icons/savewhite.svg"), QIcon(":/icons/saveblue.svg")});
    m_iconMap.insert("closeButton", {QIcon(":/icons/closewhite.svg"), QIcon(":/icons/closered.svg")});
    m_iconMap.insert("refreshButton", {QIcon(":/icons/refreshwhite.svg"), QIcon(":/icons/refreshblue.svg")});
    m_iconMap.insert("homeButton", {QIcon(":/icons/homewhite.svg"), QIcon(":/icons/home.svg")});
    m_iconMap.insert("usageButton", {QIcon(":/icons/storagewhite.svg"), QIcon(":/icons/storage.svg")});
    m_iconMap.insert("trashButton", {QIcon(":/icons/deletewhite.svg"), QIcon(":/icons/delete.svg")});
    m_iconMap.insert("addfileButton", {QIcon(":/icons/addfilewhite.svg"), QIcon(":/icons/addfile.svg")});
    m_iconMap.insert("addfolderButton", {QIcon(":/icons/addfolderwhite.svg"), QIcon(":/icons/addfolder.svg")});
    m_iconMap.insert("logoutButton", {QIcon(":/icons/logoutwhite.svg"), QIcon(":/icons/logout.svg")});
    m_iconMap.insert("saveButton", {QIcon(":/icons/savewhite.svg"), QIcon(":/icons/save.svg")});
    m_iconMap.insert("saveButton_2", {QIcon(":/icons/savewhite.svg"), QIcon(":/icons/save.svg")});


    connect(this, &QToolButton::clicked, this, &HoverButton::handleClicked);
}

void HoverButton::handleClicked() {
    for (auto &name : m_exceptions) {
        if (this->objectName() == name) {
            hoverState();
        }
    }
}

void HoverButton::handleHoverEvent(QHoverEvent* event) {
    if (m_iconMap.contains(this->objectName()) && m_state.isNormal()) {
        if (event->type() == QEvent::HoverEnter) {
            hoverState();
            emit enterHover();
        } else if (event->type() == QEvent::HoverLeave && m_state.isNormal()) {
            leaveState();
            emit leaveHover();
        }
    }
}

void HoverButton::hoverState() {
    if (m_iconMap.contains(this->objectName())) {
        setIcon(m_iconMap.value(this->objectName()).second);
    }
    setIconSize(iconSize());
}

void HoverButton::leaveState() {
    if (m_iconMap.contains(this->objectName())) {
        setIcon(m_iconMap.value(this->objectName()).first);
    }
    setIconSize(iconSize());
}

void HoverButton::animateColor(const QColor &startValue, const QColor &endValue) {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
