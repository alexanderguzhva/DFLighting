TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    random.cpp

QMAKE_CXXFLAGS += "-std=c++11"
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -mavx -funroll-loops -march=native
#QMAKE_CXXFLAGS += -pg -g
#QMAKE_LFLAGS += -pg -g
#QMAKE_CXXFLAGS += -g
#QMAKE_LFLAGS += -g
#QMAKE_CXXFLAGS_RELEASE += -fprofile-generate
#QMAKE_LFLAGS_RELEASE += -fprofile-generate

#QMAKE_CXXFLAGS_RELEASE += -fprofile-use
#QMAKE_LFLAGS_RELEASE += -fprofile-use

QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

INCLUDEPATH += /usr/include/SDL
LIBS += -lSDL -lSDL_image -lSDL_gfx


HEADERS += \
    random.h \
    mydoublelinkedlistelement.h \
    mydoublelinkedlist.h


