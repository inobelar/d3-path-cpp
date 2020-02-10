TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD

include($$PWD/../src/d3_path.pri)

SOURCES += \
    path-test.cpp

HEADERS += \
    pathEqual.hpp
