#-------------------------------------------------
#
# Project created by QtCreator 2010-07-03T14:15:53
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = MumblePAHelper
TEMPLATE = app

SOURCES += main.cpp\
        mumblepahelper.cpp \
    Plugins.cpp

HEADERS  += mumblepahelper.h \
    Plugins.h

FORMS    += mumblepahelper.ui

win32:RC_ICONS += resources/MumblePAHelper.ico

CONFIG += c++11

CONFIG(debug, debug|release) {
  CONFIG += console
  DESTDIR   = $$_PRO_FILE_PWD_/debug
}

CONFIG(release, debug|release) {
  DESTDIR   = $$_PRO_FILE_PWD_/release
}

QMAKE_LIBDIR += $$DESTDIR
OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui
