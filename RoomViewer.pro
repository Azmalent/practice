#-------------------------------------------------
#
# Project created by QtCreator 2019-02-06T22:57:41
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RoomViewer
TEMPLATE = app

QMAKE_CXXFLAGS +=

INCLUDEPATH += \
    "C:\sdk\assimp\include"         \
    "C:\sdk\assimp-4.0.1\include"   \
    "C:\sdk\irrxml-1.2\src"         \
    "C:\sdk\Python36\include"

LIBS += -lglu32 -lopengl32           \
    "C:\sdk\assimp\code\libassimp.a" \
    "C:\Program Files\Haskell Platform\8.2.2\mingw\lib\libz.a"

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
    main.cpp            \
    mainwindow.cpp      \
    openglview.cpp      \
    modelimporter.cpp

HEADERS += \
    mainwindow.h    \
    openglview.h    \
    modelinfo.h     \
    modelimporter.h

FORMS += mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
