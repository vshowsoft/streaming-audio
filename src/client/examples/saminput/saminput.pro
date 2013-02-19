#-------------------------------------------------
#
# Project created by QtCreator 2012-11-16T15:50:58
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = saminput
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

ParentDirectory = ../../../..

UI_DIR = "$$ParentDirectory/build"
MOC_DIR = "$$ParentDirectory/build"
OBJECTS_DIR = "$$ParentDirectory/build"

CONFIG(debug, debug|release) {
    DESTDIR = "$$ParentDirectory/bin/debug"
}
CONFIG(release, debug|release) {
    DESTDIR = "$$ParentDirectory/bin"
}

SOURCES += main.cpp

INCLUDEPATH += /usr/local/include $$ParentDirectory/src $$ParentDirectory/src/client
LIBS += -L$$ParentDirectory/lib -lsac -ljack
QT += network

message(saminput.pro complete)
