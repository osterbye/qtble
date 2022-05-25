QT -= gui

VERSION = 1.0.0
TEMPLATE = lib
DEFINES += QTBLE_LIBRARY

CONFIG += c++11
QMAKE_CXXFLAGS += -Wall -Wextra
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += \
    include/ \
    src/ \
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
    src/advertising.cpp \
    src/blecharacteristic.cpp \
    src/bleservice.cpp \
    src/btatt.cpp \
    src/btsocket.cpp \
    src/btuuid.cpp \
    src/gattserver.cpp \
    src/qtble.cpp \
    src/qtblevalue.cpp

HEADERS += \
    src/advertising.h \
    src/blecharacteristic.h \
    src/bleservice.h \
    src/btatt.h \
    src/btsocket.h \
    src/btuuid.h \
    src/gattserver.h \
    src/gattservice.h

LIBS += -lbluetooth -lshared-glib

DESTDIR = $$_PRO_FILE_PWD_/../bin/
OBJECTS_DIR = $$_PRO_FILE_PWD_/../build/.obj
MOC_DIR = $$_PRO_FILE_PWD_/../build/.moc
RCC_DIR = $$_PRO_FILE_PWD_/../build/.qrc
UI_DIR = $$_PRO_FILE_PWD_/../build/.ui
