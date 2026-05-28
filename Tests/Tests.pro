QT       += testlib core gui widgets network sql
CONFIG   += c++17 console
CONFIG   -= app_bundle

TARGET = SmartMonitorTests

# 被测源码路径
SRC_DIR = ../SmartMonitorApp

INCLUDEPATH += $$SRC_DIR
INCLUDEPATH += $$SRC_DIR/model
INCLUDEPATH += $$SRC_DIR/network
INCLUDEPATH += $$SRC_DIR/database

SOURCES += \
    main.cpp \
    tst_dataparser.cpp \
    tst_dbmanager.cpp \
    $$SRC_DIR/network/DataParser.cpp \
    $$SRC_DIR/database/DbManager.cpp

HEADERS += \
    tst_dataparser.h \
    tst_dbmanager.h \
    $$SRC_DIR/network/DataParser.h \
    $$SRC_DIR/model/DeviceInfo.h \
    $$SRC_DIR/model/AlarmRecord.h \
    $$SRC_DIR/database/DbManager.h
