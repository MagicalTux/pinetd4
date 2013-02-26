TEMPLATE = lib
CONFIG += shared
TARGET = ../core
DEPENDPATH += .
INCLUDEPATH += .
DEFINES += PINETD_CORE_LIBRARY

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp
