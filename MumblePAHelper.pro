# Copyright 2005-2018 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

QT       += core gui widgets

TARGET = MumblePAHelper
TEMPLATE = app

win32 {
  isEqual(QT_MAJOR_VERSION, 5) {
    QTPLUGIN += qwindows
  }
}

SOURCES += main.cpp\
        mumblepahelper.cpp \
    Plugins.cpp

HEADERS  += mumblepahelper.h \
    Plugins.h

FORMS    += mumblepahelper.ui

win32:RC_ICONS += resources/MumblePAHelper.ico

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
