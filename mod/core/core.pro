TEMPLATE = lib
CONFIG += shared
TARGET = ../core
DEPENDPATH += .
INCLUDEPATH += . ../../include
DEFINES += PINETD_CORE_LIBRARY

QT += network
QT -= gui

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

QMAKE_CXXFLAGS += -std=gnu++0x

# Input
SOURCES += main.cpp Core.cpp CoreTcp.cpp Daemon.cpp Client.cpp ClientTcp.cpp
HEADERS += ../../include/core/Core.hpp ../../include/core/CoreTcp.hpp
HEADERS += ../../include/core/Daemon.hpp
HEADERS += ../../include/core/Client.hpp ../../include/core/ClientTcp.hpp
