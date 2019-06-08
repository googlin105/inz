#-------------------------------------------------
#
# Project created by QtCreator 2019-05-28T11:23:43
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = projekt
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
        qcustomplot.cpp \
        secdialog.cpp

HEADERS += \
        mainwindow.h \
        qcustomplot.h \
        secdialog.h

FORMS += \
        mainwindow.ui \
        secdialog.ui

LIBS += \
       -lboost_system\
        -lboost_program_options\
        -lboost_thread\
         /usr/local/lib/libfftw3.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../../../../usr/local/lib/ -luhd

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

INCLUDEPATH += /usr/local/include/uhd/usrp
INCLUDEPATH += /usr/local/include/uhd/utils
INCLUDEPATH += /usr/local/include/uhd
INCLUDEPATH += /usr/local/include/uhd/types

