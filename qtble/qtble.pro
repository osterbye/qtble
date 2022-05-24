QT -= gui

VERSION = 1.0.0
TEMPLATE = lib
DEFINES += QTBLE_LIBRARY

CONFIG += c++11
QMAKE_CXXFLAGS += -Wall -Wextra
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += \
    include/ \
    bluez-5.41/

# Public headers
HEADERS += \
    include/qtble_global.h \
    include/qtble_types.h \
    include/qtble.h \
    include/qtbleauth.h \
    include/qtblevalue.h

# Source
SOURCES += \
    src/qtble.cpp \
    src/qtblevalue.cpp

LIBS += -lbluetooth -lshared-glib

DESTDIR = $$_PRO_FILE_PWD_/../bin/
OBJECTS_DIR = $$_PRO_FILE_PWD_/../build/.obj
MOC_DIR = $$_PRO_FILE_PWD_/../build/.moc
RCC_DIR = $$_PRO_FILE_PWD_/../build/.qrc
UI_DIR = $$_PRO_FILE_PWD_/../build/.ui
