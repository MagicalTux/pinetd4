TEMPLATE = lib
CONFIG += shared
TARGET = ../qamqp
DEPENDPATH += .
INCLUDEPATH += . ../../../include ../../../include/ext/qamqp

QT -= gui
QT += network

# allows qt to not create a bunch of useless symlinks
CONFIG += plugin

# Input

HEADERS += ../../../include/ext/qamqp/amqp.h \
           ../../../include/ext/qamqp/amqp_authenticator.h \
           ../../../include/ext/qamqp/amqp_channel.h \
           ../../../include/ext/qamqp/amqp_channel_p.h \
           ../../../include/ext/qamqp/amqp_connection.h \
           ../../../include/ext/qamqp/amqp_connection_p.h \
           ../../../include/ext/qamqp/amqp_exchange.h \
           ../../../include/ext/qamqp/amqp_exchange_p.h \
           ../../../include/ext/qamqp/amqp_frame.h \
           ../../../include/ext/qamqp/amqp_message.h \
           ../../../include/ext/qamqp/amqp_network.h \
           ../../../include/ext/qamqp/amqp_p.h \
           ../../../include/ext/qamqp/amqp_queue.h \
           ../../../include/ext/qamqp/amqp_queue_p.h \
           ../../../include/ext/qamqp/amqp_global.h \

SOURCES += amqp.cpp \
           amqp_authenticator.cpp \
           amqp_channel.cpp \
           amqp_connection.cpp \
           amqp_exchange.cpp \
           amqp_frame.cpp \
           amqp_network.cpp \
           amqp_queue.cpp \
