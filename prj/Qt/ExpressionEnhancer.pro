TEMPLATE = app

CONFIG -= app_bundle
CONFIG += console c++11 warn_on c++14

QT += core


SOURCES += ../../main.cpp \
            ../../enhancer.cpp \
            ../../parser.cpp \
            ../../tinyxml2.cpp

HEADERS += ../../common.h \
            ../../enhancer.h \
            ../../parser.h \
            ../../tinyxml2.h

DISTFILES += ../../test_001.xml
