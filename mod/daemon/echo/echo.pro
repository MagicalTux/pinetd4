TEMPLATE = lib
CONFIG += shared
TARGET = ../echo
DEPENDPATH += .
INCLUDEPATH += . ../../../include
#LIBS = -L../.. -lcore

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModEcho.cpp
HEADERS += ModEcho.hpp
