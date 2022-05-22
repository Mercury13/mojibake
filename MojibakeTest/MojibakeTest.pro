TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../GoogleTest/src/gtest-all.cc \
    ../GoogleTest/src/gtest_main.cc \
    test_Mojibake.cpp

HEADERS += \
    ../GoogleTest/include/gtest/gtest.h \
    ../include/mojibake.h

INCLUDEPATH += \
    ../GoogleTest \
    ../GoogleTest/include \
    ../include
