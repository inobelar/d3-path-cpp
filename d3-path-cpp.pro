TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include($$PWD/src/d3_path.pri)

SOURCES += \
        main.cpp
