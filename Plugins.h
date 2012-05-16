/* Copyright (C) 2005-2011, Thorvald Natvig <thorvald@natvig.com>
   Copyright (C) 2010, Stefan Hacker <dd0t@users.sourceforge.net>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#include <QtCore>
#include <string>

#ifdef Q_OS_WIN
#include <windows.h>
#include <Tlhelp32.h>
#endif

#include "../mumble/plugins/mumble_plugin.h"

struct PluginInfo {
	bool locked;
	bool enabled;
	QLibrary lib;
	QString filename;
	QString description;
	QString shortname;
	MumblePlugin *p;
	MumblePlugin2 *p2;
	PluginInfo();
};

class Plugins : public QObject {
	friend class MumblePAHelper;
	private:
		Q_OBJECT
		Q_DISABLE_COPY(Plugins)
	protected:
		QReadWriteLock qrwlPlugins;
		QList<PluginInfo *> qlPlugins;
		PluginInfo *locked;
		PluginInfo *prevlocked;
		void clearPlugins();
		int iPluginTry;
		QMap<QString, QString> qmPluginHash;
		QString qsUserPlugins;
		QString qsSystemPlugins;
	public:
		std::string ssContext, ssContextSent;
		std::wstring swsIdentity, swsIdentitySent;
		bool bValid;
		bool bUnlink;
		float fPosition[3], fFront[3], fTop[3];
		float fCameraPosition[3], fCameraFront[3], fCameraTop[3];

		Plugins(QObject *p = NULL);
		~Plugins();
	public slots:
		void on_Timer_timeout();
		void rescanPlugins();
		bool fetch();
	signals:
		void Fetched();
		void IdentityChanged(const QString);
		void ContextChanged(const QString);
		void LinkLost(const PluginInfo*);
		void Linked(const PluginInfo*);
		void PluginList(const QList<PluginInfo*>);
};

#endif
