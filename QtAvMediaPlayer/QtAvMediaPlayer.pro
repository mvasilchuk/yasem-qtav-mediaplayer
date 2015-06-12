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
    qtavmediaplayerobject.cpp \
    $${CORE_ROOT_DIR}/mediaplayerpluginobject.cpp

HEADERS +=\
    ../QtAV/widgets/QtAVWidgets/WidgetRenderer.h \
    ../QtAV/src/QtAV/AVPlayer.h \
    $${CORE_ROOT_DIR}/mediaplayerpluginobject.h \
    qtavmediaplayer_global.h \
    qtavmediaplayerplugin.h \
    qtavmediaplayerobject.h \
    $${CORE_ROOT_DIR}/browserpluginobject.h \
    $${CORE_ROOT_DIR}/guipluginobject.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!mac {
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../libs
  QMAKE_RPATH=
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_DIR -lQt5AV -lQt5AVWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_DIR -lQt5AVd -lQt5AVWidgetsd
else:unix: {
    #greaterThan(QT_MINOR_VERSION, 3): LIBS += -L$$OUT_DIR/libs -lQt5AV -lQt5AVWidgets
    #else:
    LIBS += -L$$OUT_DIR/libs -lQtAV -lQtAVWidgets
}

OTHER_FILES += \
    metadata.json 

DEPENDENCY_LIBRARIES = QtAV

DISTFILES += \
    qtav_destdir_patch.pri \
    qtav_libdir_patch.pri
