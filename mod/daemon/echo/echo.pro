TEMPLATE = lib
CONFIG += shared
TARGET = ../echo
DEPENDPATH += .
INCLUDEPATH += . ../../../include
#LIBS = -L../.. -lcore

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModEcho.cpp ModEchoClient.cpp
HEADERS += ModEcho.hpp ModEchoClient.hpp
