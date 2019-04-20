#-------------------------------------------------
#
# Project created by QtCreator 2019-04-01T10:49:01
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mesh
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

win32: {
    message("win build")

    LIBS += opengl32.lib

    INCLUDEPATH += C:/LeapSDK/include

    !contains(QMAKE_HOST.arch, x86_64) {
        message("win x86 build")
        LIBS += C:/LeapSDK/lib/x86/Leap.lib
    } else {
        message("win x86_64 build")
        LIBS += C:/LeapSDK/lib/x64/Leap.lib
    }
}

macx: {
    message("mac build")
    INCLUDEPATH += /Users/zclin/Documents/LeapSDK/include

    LIBS += -L/Users/zclin/Documents/LeapSDK/lib/ -lLeap
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    glwidget.cpp

HEADERS += \
        mainwindow.h \
    glwidget.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
