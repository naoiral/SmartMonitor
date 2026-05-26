QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = DeviceSimulator

SOURCES += \
    main.cpp \
    SimMainWindow.cpp \
    DeviceServer.cpp

HEADERS += \
    SimMainWindow.h \
    DeviceServer.h
