TEMPLATE = app

CONFIG += console
CONFIG += c++11
CONFIG += warn_on
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += ../../main.cpp \
            ../../enhancer.cpp \
            ../../parser.cpp \
            ../../tinyxml2.cpp

HEADERS += ../../common.h \
            ../../enhancer.h \
            ../../parser.h \
            ../../tinyxml2.h

DISTFILES += ../../test_001.xml
