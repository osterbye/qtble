QT -= gui

VERSION = 1.0.0
TEMPLATE = lib
DEFINES += QTBLE_LIBRARY

QMAKE_CXXFLAGS += -Wall -Wextra

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

# BlueZ shared LGPL
SOURCES += \
    $$_PRO_FILE_PWD_/../bluez/src/shared/att.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/crypto.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/gatt-db.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/gatt-server.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/io-glib.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/queue.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/timeout-glib.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/util.c \
    $$_PRO_FILE_PWD_/../bluez/src/shared/uuid.c

HEADERS += \
    $$_PRO_FILE_PWD_/../bluez/src/shared/att.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/bluetooth-definitions.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/crypto.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/gatt-db.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/gatt-server.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/gatt-helpers.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/io.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/l2cap-definitions.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/queue.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/timeout.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/util.h \
    $$_PRO_FILE_PWD_/../bluez/src/shared/uuid.h


DESTDIR = $$_PRO_FILE_PWD_/../bin/
OBJECTS_DIR = $$_PRO_FILE_PWD_/../build/.obj
MOC_DIR = $$_PRO_FILE_PWD_/../build/.moc
RCC_DIR = $$_PRO_FILE_PWD_/../build/.qrc
UI_DIR = $$_PRO_FILE_PWD_/../build/.ui
