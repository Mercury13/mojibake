TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

SOURCES += \
    ../GoogleTest/src/gtest-all.cc \
    ../GoogleTest/src/gtest_main.cc \
    test_Mojibake.cpp

HEADERS += \
    ../include/mojibake.h \
    ../include/internal/detail.hpp

INCLUDEPATH += \
    ../GoogleTest \
    ../GoogleTest/include \
    ../include
