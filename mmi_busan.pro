#-------------------------------------------------
#
# Project created by QtCreator 2022-07-15T22:40:03
#
#-------------------------------------------------

QT       += core gui serialport multimedia xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mmi_busan
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    speedgauge.cpp \
    imagebutton.cpp \
    dialogmode.cpp \
    touchtranslator.cpp \
    dialogconfig.cpp \
    dialogprotocol.cpp \
    dialogupdate.cpp \
    dialogmsg.cpp \
    dialogpassword.cpp

HEADERS += \
        mainwindow.h \
    speedgauge.h \
    imagebutton.h \
    dialogmode.h \
    touchtranslator.h \
    dialogconfig.h \
    dialogprotocol.h \
    dialogupdate.h \
    dialogmsg.h \
    dialogpassword.h

FORMS += \
        mainwindow.ui \
    dialogmode.ui \
    dialogconfig.ui \
    dialogprotocol.ui \
    dialogupdate.ui \
    dialogmsg.ui \
    dialogpassword.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
