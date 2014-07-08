#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T21:33:44
#
#-------------------------------------------------




QT       = core gui widgets

TARGET = yasem-qtav-mediaplayer
TEMPLATE = lib

DEFINES += LIBAVMEDIAPLAYER_LIBRARY

INCLUDEPATH += ../../../yasem-core \
    ../QtAV/src/

SOURCES += libavmediaplayer.cpp

HEADERS += libavmediaplayer.h\
        libavmediaplayer_global.h \
        ../QtAV/src/QtAV/WidgetRenderer.h \
        ../QtAV/src/QtAV/AVPlayer.h \
        ../../../yasem-core/mediasignalsender.h \
        playerthread.h

unix {
    target.path = /usr/lib
    INSTALLS += targe
}

BUILD_DIR=$$OUT_PWD/../../../
message($$BUILD_DIR)



unix:!mac{
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../libs
  QMAKE_RPATH=
}


LIBS += -L$$BUILD_DIR/bin/libs -lQtAV

OTHER_FILES += \
    metadata.json 

#include(../../../common.pri)
#DESTDIR = D_DIR

DESTDIR=../../../bin/plugins

DEPENDENCY_LIBRARIES = QtAV

#include(../QtAV/src/libQtAV.pri)

