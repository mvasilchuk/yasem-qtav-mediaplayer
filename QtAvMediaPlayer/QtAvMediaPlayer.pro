#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T21:33:44
#
#-------------------------------------------------


VERSION = 0.1.0

QT       = core gui widgets

CONFIG += ordered

CONFIG += c++11

TARGET = yasem-qtav-mediaplayer
TEMPLATE = lib

DEFINES += QTAVMEDIAPLAYER_LIBRARY

INCLUDEPATH += ../../../yasem-core \
    ../QtAV/src/ \
    ../QtAV/widgets/

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
else:unix: {
    greaterThan(QT_MINOR_VERSION, 3): LIBS += -L$$BUILD_DIR/bin/libs -lQt5AV -lQt5AVWidgets
    else: LIBS += -L$$BUILD_DIR/bin/libs -lQtAV -lQtAVWidgets
}

OTHER_FILES += \
    metadata.json 

DESTDIR=../../../bin/plugins

DEPENDENCY_LIBRARIES = QtAV
