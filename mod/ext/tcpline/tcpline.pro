TEMPLATE = lib
CONFIG += shared
TARGET = ../tcpline
DEPENDPATH += .
INCLUDEPATH += . ../../../include

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += ClientTcpLine.cpp
HEADERS += ../../../include/ext/ClientTcpLine.hpp
