TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    sparsematrix.cpp \
    paramparser.cpp

HEADERS += \
    matrixGenerator.h \
    paramparser.h \
    sparsematrix.h

LIBS += -lpthread

CONFIG += c++11

