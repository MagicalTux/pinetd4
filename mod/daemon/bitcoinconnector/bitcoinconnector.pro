TEMPLATE = lib
CONFIG += shared
TARGET = ../bitcoinconnector
DEPENDPATH += .
INCLUDEPATH += . ../../../include

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input
SOURCES += main.cpp ModBitcoinConnector.cpp ModBitcoinConnectorClient.cpp
HEADERS += ModBitcoinConnector.hpp ModBitcoinConnectorClient.hpp
