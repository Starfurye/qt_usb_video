#-------------------------------------------------
#
# Project created by QtCreator 2019-01-31T14:51:55
#
#-------------------------------------------------

QT       += core gui
QT       += core gui printsupport
QT       += core gui opengl
QT       += widgets
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_usb_video
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qextserialbase.cpp \
    qextserialport.cpp \
    win_qextserialport.cpp \
    scenewindow.cpp \
    inputdialog.cpp \
    modifydialog.cpp \
    updatedialog.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.hpp \
    qextserialbase.h \
    qextserialport.h \
    win_qextserialport.h \
    scenewindow.hpp \
    inputdialog.h \
    modifydialog.h \
    updatedialog.h \
    qcustomplot.h

FORMS    += mainwindow.ui \
    inputdialog.ui \
    modifydialog.ui \
    updatedialog.ui

LIBS += -lopengl32  -lglu32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/bulkloop/lib/x64/ -lCyAPI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/bulkloop/lib/x64/ -lCyAPI
else:unix: LIBS += -L$$PWD/bulkloop/lib/x64/ -lCyAPI

INCLUDEPATH += $$PWD/bulkloop/inc
DEPENDPATH += $$PWD/bulkloop/inc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/opencv2/x64/vc15/lib/ -lopencv_world401
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/opencv2/x64/vc15/lib/ -lopencv_world401d
else:unix: LIBS += -L$$PWD/opencv2/x64/vc15/lib/ -lopencv_world401

INCLUDEPATH += $$PWD/opencv2
DEPENDPATH += $$PWD/opencv2


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lSetupapi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lSetupapi
else:unix: LIBS += -L$$PWD/ -lSetupapi

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -llegacy_stdio_definitions
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -llegacy_stdio_definitions
else:unix: LIBS += -L$$PWD/ -llegacy_stdio_definitions

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lUser32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lUser32
else:unix: LIBS += -L$$PWD/ -lUser32




INCLUDEPATH += $$PWD/bulkloop/lib/x64
DEPENDPATH += $$PWD/bulkloop/lib/x64

DISTFILES += \
    shader/simple.frag \
    shader/simple.vert \
    shader/notebook


