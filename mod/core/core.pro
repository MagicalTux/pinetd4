TEMPLATE = lib
CONFIG += shared
TARGET = ../core
DEPENDPATH += .
INCLUDEPATH += . ../../include
DEFINES += PINETD_CORE_LIBRARY

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

QMAKE_CXXFLAGS += -std=gnu++0x

# Input
SOURCES += main.cpp Core.cpp Daemon.cpp
HEADERS += ../../include/mod/core/Core.hpp ../../include/mod/core/Daemon.hpp
