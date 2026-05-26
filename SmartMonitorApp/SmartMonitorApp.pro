QT       += core gui widgets network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    ui/LoginWindow.cpp \
    ui/MainWindow.cpp \
    ui/DeviceDetail.cpp \
    ui/HistoryQuery.cpp \
    network/TcpClient.cpp \
    network/DataParser.cpp \
    thread/RecvThread.cpp \
    database/DbManager.cpp \
    database/DataExporter.cpp

HEADERS += \
    model/DeviceInfo.h \
    model/AlarmRecord.h \
    ui/LoginWindow.h \
    ui/MainWindow.h \
    ui/DeviceDetail.h \
    ui/HistoryQuery.h \
    network/TcpClient.h \
    network/DataParser.h \
    thread/RecvThread.h \
    database/DbManager.h \
    database/DataExporter.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resource.qrc
