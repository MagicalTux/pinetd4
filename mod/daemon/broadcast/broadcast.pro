TEMPLATE = lib
CONFIG += shared
TARGET = ../broadcast
DEPENDPATH += .
INCLUDEPATH += . ../../../include

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModBroadcast.cpp ModBroadcastClient.cpp
HEADERS += ModBroadcast.hpp ModBroadcastClient.hpp
