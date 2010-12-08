# #####################################################################
# Automatically generated by qmake (2.01a) Wed Aug 26 09:43:23 2009
# #####################################################################
TEMPLATE = lib
TARGET = qmsystem2
DEPENDPATH += .
INCLUDEPATH += .
DEFINES += SYSTEM_LIBRARY
QT += dbus
QT += network
QT -= gui

QMAKE_CXXFLAGS += -Wall -Wno-psabi

CONFIG += timed
CONFIG += link_pkgconfig
PKGCONFIG += bmeipc dsme dsme_dbus_if gconf-2.0 libiphb sensord usb_moded

# Input
HEADERS += mainpage.h \
    msystemdbus.h \
    qmaccelerometer.h \
    qmaccelerometer_p.h \
    qmactivity.h \
    qmactivity_p.h \
    qmals.h \
    qmals_p.h \
    qmbattery.h \
    qmbattery_p.h \
    qmcabc.h \
    qmcallstate.h \
    qmcallstate_p.h \
    qmcompass.h \
    qmcompass_p.h \
    qmdevicemode.h \
    qmdevicemode_p.h \
    qmdisplaystate.h \
    qmdisplaystate_p.h \
    qmheartbeat.h \
    qmheartbeat_p.h \
    qmipcinterface.h \
    qmkeys.h \
    qmkeys_p.h \
    qmled.h \
    qmlocks.h \
    qmlocks_p.h \
    qmmagnetometer.h \
    qmmagnetometer_p.h \
    qmorientation.h \
    qmorientation_p.h \
    qmproximity.h \
    qmproximity_p.h \
    qmrotation.h \
    qmrotation_p.h \
    qmsensor.h \
    qmsensor_p.h \
    qmsystemstate.h \
    qmsystemstate_p.h \
    qmtap.h \
    qmtap_p.h \
    qmthermal.h \
    qmthermal_p.h \
    qmtime.h \
    qmusbmode.h \
    qmusbmode_p.h \
    qmwatchdog.h \
    qmwatchdog_p.h \
    sysfs-watcher.h \
    system_global.h \
    qmtime_p.h
SOURCES += qmactivity.cpp \
    qmaccelerometer.cpp \ 
    qmals.cpp \
    qmbattery.cpp \
    qmcabc.cpp \
    qmcallstate.cpp \
    qmcompass.cpp \
    qmdevicemode.cpp \
    qmdisplaystate.cpp \
    qmheartbeat.cpp \
    qmipcinterface.cpp \
    qmkeys.cpp \
    qmled.cpp \
    qmlocks.cpp \
    qmorientation.cpp \
    qmsystemstate.cpp \
    qmtap.cpp \
    qmthermal.cpp \
    qmproximity.cpp \
    qmtime.cpp \
    sysfs-watcher.cpp \
    qmsensor.cpp \
    qmrotation.cpp \
    qmmagnetometer.cpp \
    qmwatchdog.cpp \
    qmusbmode.cpp
contextsubscriber { 
    DEFINES += PROVIDE_CONTEXT_INFO
    LIBS += -lcontextsubscriber
}
targetheaders.path = /usr/include/qmsystem2
targetheaders.files = $$HEADERS
target.path = /usr/lib/
INSTALLS += target \
    targetheaders
