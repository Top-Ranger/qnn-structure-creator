#-------------------------------------------------
#
# Project created by QtCreator 2015-12-22T00:48:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qnn-structure-creator
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += src/main.cpp\
        src/qnnstructurecreator.cpp

HEADERS  += src/qnnstructurecreator.h

FORMS    += src/qnnstructurecreator.ui

unix: LIBS += -L$$PWD/../qnn/ -lqnn
win32: LIBS += -L$$PWD/../qnn/ -lqnn0

INCLUDEPATH += $$PWD/../qnn/src
DEPENDPATH += $$PWD/../qnn/src

OTHER_FILES += \
    LICENSE.GPL3

DISTFILES += \
    README
