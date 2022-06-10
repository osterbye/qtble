TARGET = qtble_simple

CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += $$_PRO_FILE_PWD_/../../qtble/include
LIBS += -L"$$_PRO_FILE_PWD_/../../bin" -lqtble

SOURCES += \
        main.cpp

DESTDIR = $$_PRO_FILE_PWD_/../../bin/
OBJECTS_DIR = $$_PRO_FILE_PWD_/../../build/.obj
MOC_DIR = $$_PRO_FILE_PWD_/../../build/.moc
RCC_DIR = $$_PRO_FILE_PWD_/../../build/.qrc
UI_DIR = $$_PRO_FILE_PWD_/../../build/.ui
