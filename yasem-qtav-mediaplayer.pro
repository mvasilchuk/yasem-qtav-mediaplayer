
#include(../../common.pri)

VERSION = 0.1.0

TEMPLATE = subdirs

CONFIG += ordered
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


