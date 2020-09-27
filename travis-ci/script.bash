#!/bin/bash -ex
#
# Copyright 2018-2020 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

git clone https://github.com/mumble-voip/mumble.git

MUMBLE_PLUGINS_PATH="${PWD}/mumble/plugins"

mkdir build && cd build

if [ "${MUMBLE_HOST}" == "x86_64-linux-gnu" ]; then
	cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPLUGIN_HEADER_DIR=${MUMBLE_PLUGINS_PATH} ..
elif [ "${MUMBLE_HOST}" == "i686-w64-mingw32" ] || [ "${MUMBLE_HOST}" == "x86_64-w64-mingw32" ]; then
	PATH=$PATH:/usr/lib/mxe/usr/bin
	${MUMBLE_HOST}.static-cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPLUGIN_HEADER_DIR=${MUMBLE_PLUGINS_PATH} ..
else
	exit 1
fi

ninja
