#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T21:33:44
#
#-------------------------------------------------

VERSION = 0.1.0
TARGET = yasem-qtav-mediaplayer
TEMPLATE = lib

include($${top_srcdir}/common.pri)

QT       = core gui widgets

CONFIG +=  debug_and_release

DEFINES += QTAVMEDIAPLAYER_LIBRARY

INCLUDEPATH += \
    ../QtAV/src/ \
    ../QtAV/widgets/

SOURCES += \
    qtavmediaplayerplugin.cpp \
    qtavmediaplayerobject.cpp

HEADERS +=\
    ../QtAV/widgets/QtAVWidgets/WidgetRenderer.h \
    ../QtAV/src/QtAV/AVPlayer.h \
    qtavmediaplayer_global.h \
    qtavmediaplayerplugin.h \
    qtavmediaplayerobject.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!mac {
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../libs
  QMAKE_RPATH=
}

win32:CONFIG(release, debug|release): {
    *-g++*: { #i686-w64-mingw32/4.9.1
        # TODO: Add workarounds for different compilers
        LIBS += -L$$OUT_DIR -lQtAV -lQtAVWidgets1
    } else {
        LIBS += -L$$OUT_DIR -lQt5AV1 -lQt5AVWidgets1
    }
}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -L$$OUT_DIR -lQt5AVd -lQt5AVWidgetsd
}
else:unix: {
    clang: {
        LIBS += -L$$OUT_DIR/libs -lQtAV -lQtAVWidgets
    } else {
        # gcc
        # I have no idea why, but sometimes gcc generates library names Qt5AV and Qt5AVWidgets,
        # but sometimes without 5 in the middle.

        greaterThan(QT_MINOR_VERSION, 3): LIBS += -L$$OUT_DIR/libs -lQt5AV -lQt5AVWidgets
        else:LIBS += -L$$OUT_DIR/libs -lQtAV -lQtAVWidgets
    }

}

OTHER_FILES += \
    metadata.json 

DEPENDENCY_LIBRARIES = QtAV

DISTFILES += \
    qtav_destdir_patch.pri \
    qtav_libdir_patch.pri
