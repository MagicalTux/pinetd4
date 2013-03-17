TEMPLATE = lib
CONFIG += shared
TARGET = ../bitcoin
DEPENDPATH += .
INCLUDEPATH += . ../../../include

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin
CONFIG += crypto

# Input
SOURCES += BitcoinNetAddr.cpp BitcoinCrypto.cpp
#HEADERS += 
