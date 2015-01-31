#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T21:33:44
#
#-------------------------------------------------


VERSION = 1.0.0

QT       = core gui widgets

CONFIG += ordered debug_and_release

CONFIG += c++11

TARGET = yasem-qtav-mediaplayer
TEMPLATE = lib

DEFINES += QTAVMEDIAPLAYER_LIBRARY

INCLUDEPATH += ../../../yasem-core \
    ../QtAV/src/ \

SOURCES += \
    qtavmediaplayer.cpp

HEADERS +=\
        ../QtAV/widgets/QtAVWidgets/WidgetRenderer.h \
        ../QtAV/src/QtAV/AVPlayer.h \
        ../../../yasem-core/mediasignalsender.h \
        playerthread.h \
        qtavmediaplayer.h \
        qtavmediaplayer_global.h

unix {
    target.path = /usr/lib
    INSTALLS += targe
}

BUILD_DIR=$$OUT_PWD/../../../
message($$BUILD_DIR)

unix:!mac {
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../libs
  QMAKE_RPATH=
}



win32:CONFIG(release, debug|release): LIBS += -L$$BUILD_DIR/bin -lQt5AV -lQt5AVWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$BUILD_DIR/bin -lQt5AVd -lQt5AVWidgetsd
else:unix: LIBS += -L$$BUILD_DIR/bin/libs -lQt5AV -lQt5AVWidgets

OTHER_FILES += \
    metadata.json 

DESTDIR=../../../bin/plugins

DEPENDENCY_LIBRARIES = QtAV
