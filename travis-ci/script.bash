#!/bin/bash -ex
#
# Copyright 2005-2018 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

if [ "${MUMBLE_QT}" == "qt4" ] && [ "${MUMBLE_HOST}" == "x86_64-linux-gnu" ]; then
	git clone https://github.com/mumble-voip/mumble.git ../mumble
	qmake -Wall "QMAKE_CXXFLAGS += -Werror"
	make -j2
elif [ "${MUMBLE_QT}" == "qt5" ] && [ "${MUMBLE_HOST}" == "x86_64-linux-gnu" ]; then
	git clone https://github.com/mumble-voip/mumble.git ../mumble
	qmake -Wall "QMAKE_CXXFLAGS += -Werror"
	make -j2
elif [ "${MUMBLE_QT}" == "qt5" ] && [ "${MUMBLE_HOST}" == "i686-w64-mingw32" ]; then
	PATH=$PATH:/usr/lib/mxe/usr/bin
	git clone https://github.com/mumble-voip/mumble.git ../mumble
	${MUMBLE_HOST}.static-qmake-qt5 -Wall "QMAKE_CXXFLAGS += -Werror"
	make -j2
elif [ "${MUMBLE_QT}" == "qt5" ] && [ "${MUMBLE_HOST}" == "x86_64-w64-mingw32" ]; then
	PATH=$PATH:/usr/lib/mxe/usr/bin
	git clone https://github.com/mumble-voip/mumble.git ../mumble
	${MUMBLE_HOST}.static-qmake-qt5 -Wall "QMAKE_CXXFLAGS += -Werror"
	make -j2
else
	exit 1
fi
