TEMPLATE = lib
CONFIG += shared
TARGET = ../vps
DEPENDPATH += .
INCLUDEPATH += . ../../../include
CONFIG += silent

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModVps.cpp
HEADERS += ModVps.hpp ModVpsInst.hpp
