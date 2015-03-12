
#include(../../common.pri)
VERSION = 0.1.0

TEMPLATE = subdirs

CONFIG += ordered debug_and_release
CONFIG += c++11

DESTDIR = $$BUILD_DIR/bin
DLLDESTDIR = $$DESTDIR/plugins

INCLUDEPATH += ../../yasem-core

SUBDIRS += \
        QtAV \
        QtAvMediaPlayer

QtAvMediaPlayer.depends=QtAV

#LIBAV_DIR=../../third_party/libav-bin
#message($$absolute_path($$LIBAV_DIR))

#QtAV.LIBS += -L$$LIBAV_DIR/lib
#QtAV.INCLUDEPATH += $$LIBAV_DIR/include
#QtAV.config += static_ffmpeg


QtAV.OUT_DIR = $$DESTDIR

QtAvMediaPlayer.depends += QtAV

OTHER_FILES += \
    LICENSE \
    README.md
