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

#include "Plugins.h"

inline QString MumbleStringToQString(const MumbleString &str) {
	Q_ASSERT(str.data != NULL);

	if (str.data != NULL) {
		return QString::fromUtf8(reinterpret_cast<const char*>(str.data), str.len);
	}

	return QString();
}

static inline QString MumbleStringToQString(const MumbleWideString &wstr) {
	Q_ASSERT(wstr.data != NULL);

	if (wstr.data != NULL) {
		return QString::fromWCharArray(wstr.data, wstr.len);
	}

	return QString();
}

PluginInfo::PluginInfo() {
	locked = false;
	enabled = false;
	p = NULL;
}

Plugins::Plugins(QObject *p)
	: QObject(p)
	, msContext{contextBuf, 0, 256}
	, mwsIdentity{identityBuf, 0, 256}
{
	// Current directory plugins
	qsCurrentDirectoryPlugins = QDir::currentPath();
	// System plugins directory
#if defined(Q_OS_WIN)
	QDir qdSystemPluginsDir_x64("C:/Program Files/Mumble/Plugins");
	QDir qdSystemPluginsDir_x86("C:/Program Files (x86)/Mumble/Plugins");
	if (qdSystemPluginsDir_x64.exists())
		qsSystemPlugins = qdSystemPluginsDir_x64.absolutePath();
	else if (qdSystemPluginsDir_x86.exists())
		qsSystemPlugins = qdSystemPluginsDir_x86.absolutePath();
	else
		qsSystemPlugins = QLatin1String("Plugins");
#elif defined(Q_OS_LINUX)
	QDir qdSystemPluginsDir("/usr/lib/mumble");
	if (qdSystemPluginsDir.exists()) {
		qsSystemPlugins = qdSystemPluginsDir.absolutePath();
	} else {
		qsSystemPlugins = QLatin1String("Plugins");
	}
#endif
	// User plugins directory
	QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	qsUserPlugins = appDataLocation + QLatin1String("/Plugins");

	QTimer *timer=new QTimer(this);
	timer->setObjectName(QLatin1String("Timer"));
	timer->start(100);
	locked = prevlocked = NULL;
	bValid = false;
	iPluginTry = 0;

	for (int i=0;i<3;i++)
		fPosition[i]=fFront[i]=fTop[i]= 0.0;
	QMetaObject::connectSlotsByName(this);
}

Plugins::~Plugins() {
	clearPlugins();
}

void Plugins::clearPlugins() {
	QWriteLocker lock(&qrwlPlugins);
	foreach(PluginInfo *pi, qlPlugins) {
		if (pi->locked)
			pi->p->unlock();
		pi->lib.unload();
		delete pi;
	}
	qlPlugins.clear();
}

void Plugins::rescanPlugins() {
	clearPlugins();

	QWriteLocker lock(&qrwlPlugins);
	prevlocked = locked = NULL;
	bValid = false;

	QDir qcd(qsCurrentDirectoryPlugins, QString(), QDir::Name, QDir::Files | QDir::Readable);
	QDir qd(qsSystemPlugins, QString(), QDir::Name, QDir::Files | QDir::Readable);
	QDir qud(qsUserPlugins, QString(), QDir::Name, QDir::Files | QDir::Readable);
	QFileInfoList libs;
	if (bUseCurrentDirPlugins)
		libs += qcd.entryInfoList();
	if (bUseSystemPlugins)
		libs += qd.entryInfoList();
	if (bUseUserPlugins)
		libs += qud.entryInfoList();
	QSet<QString> loaded;
	foreach(const QFileInfo &libinfo, libs) {
		QString fname = libinfo.fileName();
		QString libname = libinfo.absoluteFilePath();
		if (!loaded.contains(fname) && QLibrary::isLibrary(libname)) {
			PluginInfo *pi = new PluginInfo();
			pi->lib.setFileName(libname);
			pi->filename = fname;
			qInfo("Plugin: %s", qPrintable(pi->filename));
			if (pi->lib.load()) {
				mumblePluginFunc mpf = reinterpret_cast<mumblePluginFunc>(pi->lib.resolve("getMumblePlugin"));
				if (mpf) {
					pi->p = mpf();
					if (pi->p && (pi->p->magic == MUMBLE_PLUGIN_MAGIC)) {
						pi->description = QString::fromWCharArray(pi->p->description);
						qInfo("Description: %s", qPrintable(pi->description));
						pi->shortname = QString::fromWCharArray(pi->p->shortname);
						qInfo("Shortname: %s", qPrintable(pi->shortname));
						pi->enabled = true;

						qlPlugins << pi;
						loaded.insert(fname);
						continue;
					}
				}
				pi->lib.unload();
			} else {
				qWarning("Plugins: Failed to load %s: %s", qPrintable(pi->filename), qPrintable(pi->lib.errorString()));
			}
			delete pi;
		}
	}
	lock.unlock();
	emit PluginList(qlPlugins);
}

bool Plugins::fetch() {
	if (! locked) {
		bValid = false;
		return bValid;
	}

	QReadLocker lock(&qrwlPlugins);
	if (! locked) {
		bValid = false;
		return bValid;
	}

	if (!locked->enabled)
		bUnlink = true;

	bool ok = locked->p->fetch(fPosition, fFront, fTop, fCameraPosition, fCameraFront, fCameraTop, &msContext, &mwsIdentity);
	if (! ok || bUnlink) {
		lock.unlock();
		QWriteLocker wlock(&qrwlPlugins);

		if (locked) {
			locked->p->unlock();
			locked->locked = false;
			prevlocked = locked;
			locked = NULL;
			for (int i=0;i<3;i++)
				fPosition[i]=fFront[i]=fTop[i]=fCameraPosition[i]=fCameraFront[i]=fCameraTop[i] = 0.0f;
		}
	}
	else {
		emit Fetched();
	}

	bValid = ok;
	return bValid;
}

static MumblePIDLookupStatus lookup_func(MumblePIDLookupContext ctx, const wchar_t *procname, unsigned long long int *pid) {
	std::multimap<std::wstring, unsigned long long int> *pids = reinterpret_cast<std::multimap<std::wstring, unsigned long long int> *>(ctx);
	std::multimap<std::wstring, unsigned long long int>::const_iterator iter = pids->find(std::wstring(procname));

	if (iter != pids->end()) {
		if (pid) {
			*pid = iter->second;
			return MUMBLE_PID_LOOKUP_OK;
		}
	}

	return MUMBLE_PID_LOOKUP_EOF;
}

void Plugins::on_Timer_timeout() {
	fetch();

	QReadLocker lock(&qrwlPlugins);

	if (prevlocked) {
		context.clear();
		identity.clear();
		emit ContextChanged(context);
		emit IdentityChanged(identity);
		emit LinkLost(prevlocked);
		prevlocked = NULL;
	}

	if (locked) {
		QString new_context, new_identity;

		new_context.reserve(256);
		new_context += QString::fromWCharArray(locked->p->shortname);
		new_context += '\0';
		new_context += MumbleStringToQString(msContext);
		if (context != new_context) {
			context = new_context;
			emit ContextChanged(context);
		}

		new_identity = MumbleStringToQString(mwsIdentity);
		if (identity != new_identity) {
			identity = new_identity;
			emit IdentityChanged(identity);
		}

		return;
	}

	lock.unlock();
	QWriteLocker wlock(&qrwlPlugins);

	if (qlPlugins.isEmpty())
		return;

	++iPluginTry;
	if (iPluginTry >= qlPlugins.count())
		iPluginTry = 0;

	std::multimap<std::wstring, unsigned long long int> pids;
#if defined(Q_OS_WIN)
	PROCESSENTRY32 pe;

	pe.dwSize = sizeof(pe);
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		BOOL ok = Process32First(hSnap, &pe);

		while (ok) {
			pids.insert(std::pair<std::wstring, unsigned long long int>(std::wstring(pe.szExeFile), pe.th32ProcessID));
			ok = Process32Next(hSnap, &pe);
		}
		CloseHandle(hSnap);
	}
#elif defined(Q_OS_LINUX)
	QDir d(QLatin1String("/proc"));
	QStringList entries = d.entryList();
	bool ok;
	foreach (const QString &entry, entries) {
		// Check if the entry is a PID
		// by checking whether it's a number.
		// If it is not, skip it.
		unsigned long long int pid = static_cast<unsigned long long int>(entry.toLongLong(&ok, 10));
		if (!ok) {
			continue;
		}

		QString exe = QFile::symLinkTarget(QString(QLatin1String("/proc/%1/exe")).arg(entry));
		QFileInfo fi(exe);
		QString firstPart = fi.baseName();
		QString completeSuffix = fi.completeSuffix();
		QString baseName;
		if (completeSuffix.isEmpty()) {
			baseName = firstPart;
		} else {
			baseName = firstPart + QLatin1String(".") + completeSuffix;
		}

		if (baseName == QLatin1String("wine-preloader") || baseName == QLatin1String("wine64-preloader")) {
			QFile f(QString(QLatin1String("/proc/%1/cmdline")).arg(entry));
			if (f.open(QIODevice::ReadOnly)) {
				QByteArray cmdline = f.readAll();
				f.close();

				int nul = cmdline.indexOf('\0');
				if (nul != -1) {
					cmdline.truncate(nul);
				}

				QString exe = QString::fromUtf8(cmdline);
				if (exe.contains(QLatin1String("\\"))) {
					int lastBackslash = exe.lastIndexOf(QLatin1String("\\"));
					if (exe.count() > lastBackslash + 1) {
						baseName = exe.mid(lastBackslash + 1);
					}
				}
			}
		}

		if (!baseName.isEmpty()) {
			pids.insert(std::pair<std::wstring, unsigned long long int>(baseName.toStdWString(), pid));
		}
	}
#endif

	PluginInfo *pi = qlPlugins.at(iPluginTry);
	if (pi->enabled) {
		if (pi->p && pi->p->trylock(lookup_func, &pids)) {
			pi->shortname = QString::fromWCharArray(pi->p->shortname);
			emit Linked(pi);
			pi->locked = true;
			bUnlink = false;
			locked = pi;
		}
	}
}
