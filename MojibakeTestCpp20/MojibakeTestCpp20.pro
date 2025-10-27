TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}
win32-clang-g++ {
    LIBS += -static -lpthread
}

SOURCES += \
    ../GoogleTest/src/gtest-all.cc \
    ../GoogleTest/src/gtest_main.cc \
    ../MojibakeTest/test_Mojibake.cpp \
    ../include/cpp/auto_casefold.cpp

HEADERS += \
    ../include/mojibake.h \
    ../include/internal/detail.hpp

INCLUDEPATH += \
    ../GoogleTest \
    ../GoogleTest/include \
    ../include
