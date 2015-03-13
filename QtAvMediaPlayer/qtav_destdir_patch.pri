
include(../../../dir_config.pri)

unix:!mac {
    DESTDIR = $$OUT_DIR/$$LIBS_OUT_DIR
}

win32: {
    DESTDIR = $$OUT_DIR
}
