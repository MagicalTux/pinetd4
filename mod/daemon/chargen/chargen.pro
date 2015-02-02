TEMPLATE = lib
CONFIG += shared
TARGET = ../chargen
DEPENDPATH += .
INCLUDEPATH += . ../../../include
CONFIG += silent

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModChargen.cpp ModChargenClient.cpp
HEADERS += ModChargen.hpp ModChargenClient.hpp
