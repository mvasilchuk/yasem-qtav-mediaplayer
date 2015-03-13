
include(../../../dir_config.pri)

unix:!mac {
    PROJECT_LIBDIR = $$OUT_DIR/$$LIBS_OUT_DIR
}

win32: {
    PROJECT_LIBDIR = $$OUT_DIR
}

