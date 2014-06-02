TEMPLATE = lib
CONFIG += shared
TARGET = ../rconnect
DEPENDPATH += .
INCLUDEPATH += . ../../../include

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModRConnect.cpp ModRConnectClient.cpp
HEADERS += ModRConnect.hpp ModRConnectClient.hpp
