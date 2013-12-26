TEMPLATE = lib
CONFIG += shared
TARGET = ../qamqp
DEPENDPATH += .
INCLUDEPATH += . ../../../include ../../../include/ext

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input

HEADERS += ../../../include/ext/amqp.h \
           ../../../include/ext/amqp_authenticator.h \
           ../../../include/ext/amqp_channel.h \
           amqp_channel_p.h \
           ../../../include/ext/amqp_connection.h \
           amqp_connection_p.h \
           ../../../include/ext/amqp_exchange.h \
           amqp_exchange_p.h \
           ../../../include/ext/amqp_frame.h \
           ../../../include/ext/amqp_message.h \
           ../../../include/ext/amqp_network.h \
           amqp_p.h \
           ../../../include/ext/amqp_queue.h \
           amqp_queue_p.h \
           ../../../include/ext/amqp_global.h \

SOURCES += amqp.cpp \
           amqp_authenticator.cpp \
           amqp_channel.cpp \
           amqp_connection.cpp \
           amqp_exchange.cpp \
           amqp_frame.cpp \
           amqp_network.cpp \
           amqp_queue.cpp \
