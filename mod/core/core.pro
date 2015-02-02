TEMPLATE = lib
CONFIG += shared
TARGET = ../core
DEPENDPATH += .
INCLUDEPATH += . ../../include
DEFINES += PINETD_CORE_LIBRARY
CONFIG += silent

QT += network
QT -= gui

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin debug

QMAKE_CXXFLAGS += -std=gnu++0x

# Input
SOURCES += main.cpp Core.cpp CoreTcp.cpp CoreUdp.cpp Daemon.cpp Client.cpp ClientTcp.cpp
unix:SOURCES += QUnixDatagramServer.cpp CoreUdg.cpp

HEADERS += ../../include/core/Core.hpp ../../include/core/CoreTcp.hpp ../../include/core/CoreUdp.hpp
HEADERS += ../../include/core/Daemon.hpp
HEADERS += ../../include/core/Client.hpp ../../include/core/ClientTcp.hpp
unix:HEADERS += ../../include/core/QUnixDatagramServer.hpp ../../include/core/CoreUdg.hpp
