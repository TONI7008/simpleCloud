QT       += core gui network multimedia concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    backgroundframe.cpp \
    inputdialog.cpp \
    main.cpp \
    home.cpp \
    mainthread.cpp \
    tclipboard.cpp \
    tdynamicframe.cpp \
    tfile.cpp \
    tfilechecker.cpp \
    tfilemanager.cpp \
    tfilewidget.cpp \
    tfolder.cpp \
    twidget.cpp \
    tblurwidget.cpp \
    tworker.cpp \
    tcloudelt.cpp \
    tframe.cpp \
    elidedlabel.cpp \
    tpushbutton.cpp \
    tlabel.cpp \
    tcompresser.cpp \
    tsecuritymanager.cpp \
    tstackedwidget.cpp \
    loginpage.cpp \
    signuppage.cpp \
    setting.cpp \
    imageworker.cpp \
    infopage.cpp \
    loadingframe.cpp \
    circularprogressbar.cpp \
    hoverbutton.cpp \
    selectoption.cpp \
    settingmanager.cpp \
    tgridlayout.cpp \
    togglebutton.cpp \
    tmenu.cpp \
    threadmanager.cpp \
    threadmanagerhelper.cpp \
    loader.cpp \
    uclient.cpp \
    dclient.cpp \
    popoutframe.cpp




HEADERS += \
    backgroundframe.h \
    inputdialog.h \
    tclipboard.h \
    tcloud.h \
    home.h \
    mainthread.h \
    tdynamicframe.h \
    tfile.h \
    tfilechecker.h \
    tfilemanager.h \
    tfilewidget.h \
    tfolder.h \
    twidget.h \
    tblurwidget.h \
    tworker.h \
    tcloudelt.h \
    tfileinfo.h \
    tframe.h \
    elidedlabel.h \
    tpushbutton.h \
    tlabel.h \
    tcompresser.h \
    tsecuritymanager.h \
    tstackedwidget.h \
    loginpage.h \
    signuppage.h \
    tlineedit.hpp \
    setting.h \
    imageworker.h \
    infopage.h \
    loadingframe.h \
    animhandler.h \
    circularprogressbar.h \
    hoverbutton.h \
    selectoption.h \
    settingmanager.h \
    tgridlayout.h \
    togglebutton.h \
    tmenu.h \
    threadmanager.h \
    threadmanagerhelper.h \
    loader.h \
    barupdater.h \
    uclient.h \
    dclient.h \
    idmaker.h \
    popoutframe.h



FORMS += \
    home.ui \
    inputdialog.ui \
    loginpage.ui \
    signuppage.ui \
    setting.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    fonts.qrc \
    icons.qrc \
    pictures.qrc \
    ressources.qrc
