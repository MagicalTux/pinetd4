TEMPLATE = lib
CONFIG += shared
TARGET = ../elasticsearch
DEPENDPATH += .
INCLUDEPATH += . ../../../include

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += ElasticSearch.cpp
HEADERS += ../../../include/ext/ElasticSearch.hpp
