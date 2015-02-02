TEMPLATE = lib
CONFIG += shared
TARGET = ../ps
DEPENDPATH += .
INCLUDEPATH += . ../../../include
CONFIG += silent

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

QMAKE_CXXFLAGS += -std=gnu++0x

# Input
SOURCES += main.cpp ModPS.cpp ModPSClient.cpp
HEADERS += ModPS.hpp ModPSClient.hpp
