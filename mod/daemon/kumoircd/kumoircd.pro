TEMPLATE = lib
CONFIG += shared
TARGET = ../kumoircd
DEPENDPATH += .
INCLUDEPATH += . ../../../include
CONFIG += silent

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModKumoIRCd.cpp ModKumoIRCdClient.cpp
HEADERS += ModKumoIRCd.hpp ModKumoIRCdClient.hpp
