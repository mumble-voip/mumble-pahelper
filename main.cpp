// Copyright 2005-2016 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include <QApplication>
#include "mumblepahelper.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setApplicationName("Mumble");
	QApplication a(argc, argv);
	MumblePAHelper w;
	w.show();

	return a.exec();
}
