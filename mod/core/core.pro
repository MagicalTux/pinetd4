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
HEADERS += ../../include/mod/core/Core.hpp ../../include/mod/core/CoreTcp.hpp
HEADERS += ../../include/mod/core/Daemon.hpp
HEADERS += ../../include/mod/core/Client.hpp ../../include/mod/core/ClientTcp.hpp
