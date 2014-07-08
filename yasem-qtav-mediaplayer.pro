
#include(../../common.pri)

VERSION = 1.0.0

TEMPLATE = subdirs

CONFIG += ordered debug_and_release
CONFIG += c++11

DESTDIR = $$BUILD_DIR/bin
DLLDESTDIR = $$DESTDIR/plugins

INCLUDEPATH += ../../yasem-core

SUBDIRS += \
        QtAV \
        QtAvMediaPlayer

QtAV.OUT_DIR = $$DESTDIR

QtAvMediaPlayer.depends += QtAV

OTHER_FILES += \
    LICENSE \
    README.md
