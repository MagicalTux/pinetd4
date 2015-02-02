TEMPLATE = lib
CONFIG += shared
TARGET = ../netflow
DEPENDPATH += .
INCLUDEPATH += . ../../../include
CONFIG += silent

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

QMAKE_CXXFLAGS += -std=gnu++0x

# Input
SOURCES += main.cpp ModNetflow.cpp
HEADERS += ModNetflow.hpp
