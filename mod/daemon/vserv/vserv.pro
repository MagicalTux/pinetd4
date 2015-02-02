TEMPLATE = lib
CONFIG += shared
TARGET = ../vserv
DEPENDPATH += .
INCLUDEPATH += . ../../../include
CONFIG += silent

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModVServ.cpp ModVServClient.cpp VServ.cpp
HEADERS += ModVServ.hpp ModVServClient.hpp VServ.hpp
