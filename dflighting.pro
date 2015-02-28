TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    random.cpp \
    egacolorsmapper.cpp \
    mapcomputecell.cpp \
    my3dface.cpp \
    my3dfacedcell.cpp \
    mapcomputecellfacedmap.cpp \
    mydoubleeqpoint.cpp \
    supportsplitstring.cpp \
    tile.cpp \
    worldmap.cpp \
    lightvisibilitycache1.cpp \
    lightvisibilitycache6.cpp \
    lightworldmap.cpp \
    astarpath.cpp \
    astarpathfinder.cpp \
    walkingagent.cpp \
    mapcomputecells.cpp \
    line3dpainter.cpp \
    universe.cpp \
    deprecated.cpp

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
    mydoublelinkedlist.h \
    visibilityeps.h \
    myintensity.h \
    mycolor.h \
    egacolorsmapper.h \
    myintpoint.h \
    myintdpoint.h \
    mydoublepoint.h \
    mapcomputecell.h \
    facedirenum.h \
    my3dface.h \
    my3dfacedcell.h \
    mapcomputecellfacedmap.h \
    mydoubleeqpoint.h \
    supportsplitstring.h \
    tilestructures.h \
    tile.h \
    worldmap.h \
    colorercell.h \
    lightvisibilitycachestruct.h \
    lightvisibilitycache1.h \
    lightvisibilitycache6.h \
    lightworldmap.h \
    astarpath.h \
    astarpriorityqueue.h \
    astarpathfinder.h \
    walkingagent.h \
    mapcomputecells.h \
    line3dpainter.h \
    universe.h \
    direction.h


