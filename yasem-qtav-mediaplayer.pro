TEMPLATE = subdirs

CONFIG += ordered debug_and_release

CONFIG += C++11

DESTDIR = $$BUILD_DIR/bin
DLLDESTDIR = $$DESTDIR/plugins


#clang {
#    message("Linux")

    SUBDIRS += \
        QtAV \
        QtAvMediaPlayer
#}

QtAV.OUT_DIR = $$DESTDIR

QtAvMediaPlayer.depends += QtAV


#include(../common.pri)
#DESTDIR = $$DEFAULT_PLUGIN_DIR
