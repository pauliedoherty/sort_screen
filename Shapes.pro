TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    asciisort.cpp \
    display.cpp

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread
LIBS += -lncurses

HEADERS += \
    asciisort.h \
    display.h
