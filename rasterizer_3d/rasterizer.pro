QT       += core gui

CONFIG += c++11

INCLUDEPATH += include
INCLUDEPATH += $$PWD

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rasterize
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    polygon.cpp \
    rasterizer.cpp \
    tiny_obj_loader.cc \
    segment.cpp \
    camera.cpp

HEADERS  += mainwindow.h \
    polygon.h \
    rasterizer.h \
    tiny_obj_loader.h \
    segment.h \
    camera.h

FORMS    += mainwindow.ui
