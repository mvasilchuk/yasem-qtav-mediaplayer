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
    qtavmediaplayerobject.h \
    $${SDK_DIR}/mediaplayer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!mac {
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../libs
  QMAKE_RPATH=
}

isEqual(QT_MAJOR_VERSION, 5):isEqual(QT_MINOR_VERSION, 4):lessThan(QT_PATCH_VERSION, 2) {
    win32:CONFIG(debug, debug|release): {
        LIBS += -L$$OUT_DIR/libs -lQt5AVd -lQt5AVWidgetsd
    } else {
        LIBS += -L$$OUT_DIR/libs -lQt5AV -lQt5AVWidgets
    }
} else {
    win32:CONFIG(debug, debug|release): {
        LIBS += -L$$OUT_DIR/libs -lQtAVd -lQtAVWidgetsd
    } else {
        LIBS += -L$$OUT_DIR/libs -lQtAV -lQtAVWidgets
    }
}

OTHER_FILES += \
    metadata.json 

DEPENDENCY_LIBRARIES = QtAV

DISTFILES += \
    qtav_destdir_patch.pri \
    qtav_libdir_patch.pri
