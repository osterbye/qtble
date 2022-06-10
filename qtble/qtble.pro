QT -= gui
QMAKE_CXXFLAGS += -Wno-deprecated-copy

VERSION = 1.0.0
TEMPLATE = lib
DEFINES += QTBLE_LIBRARY

QMAKE_CXXFLAGS += -Wall -Wextra
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

INCLUDEPATH += \
    include/ \
    src/ \
    $$_PRO_FILE_PWD_/../bluez/

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
    src/gapservice.cpp \
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
    src/gapservice.h \
    src/gattserver.h \
    src/gattservice.h

SOURCES += \
    $$_PRO_FILE_PWD_/../bluez/src/shared/att.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/crypto.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/gatt-db.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/gatt-server.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/io-glib.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/queue.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/timeout-glib.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/util.c

SOURCES += \
    $$_PRO_FILE_PWD_/../bluez/lib/uuid.c \

DESTDIR = $$_PRO_FILE_PWD_/../bin/
OBJECTS_DIR = $$_PRO_FILE_PWD_/../build/.obj
MOC_DIR = $$_PRO_FILE_PWD_/../build/.moc
RCC_DIR = $$_PRO_FILE_PWD_/../build/.qrc
UI_DIR = $$_PRO_FILE_PWD_/../build/.ui
