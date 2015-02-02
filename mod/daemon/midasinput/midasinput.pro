TEMPLATE = lib
CONFIG += shared
TARGET = ../midasinput
DEPENDPATH += .
INCLUDEPATH += . ../../../include
CONFIG += silent

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModMidasInput.cpp
HEADERS += ModMidasInput.hpp
