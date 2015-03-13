
VERSION = 0.1.0

TEMPLATE = subdirs

CONFIG += ordered debug_and_release
CONFIG += c++11

DESTDIR = $$BUILD_DIR/bin
DLLDESTDIR = $$DESTDIR/plugins

SUBDIRS += \
        QtAV \
        QtAvMediaPlayer

QtAvMediaPlayer.depends += QtAV

OTHER_FILES += \
    LICENSE \
    README.md
