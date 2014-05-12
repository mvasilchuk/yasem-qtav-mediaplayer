TEMPLATE = subdirs

CONFIG += ordered debug_and_release

CONFIG += C++11

DESTDIR = $$BUILD_DIR/bin
DLLDESTDIR = $$DESTDIR/plugins


SUBDIRS += \
        QtAV \
        QtAvMediaPlayer

QtAV.OUT_DIR = $$DESTDIR

QtAvMediaPlayer.depends += QtAV

OTHER_FILES += \
    LICENSE \
    README.md
