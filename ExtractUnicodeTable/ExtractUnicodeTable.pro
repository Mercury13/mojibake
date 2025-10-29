TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        PugiXml/pugixml.cpp \
        main.cpp

HEADERS += \
    PugiXml/pugiconfig.hpp \
    PugiXml/pugixml.hpp

INCLUDEPATH += \
    PugiXml

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}
