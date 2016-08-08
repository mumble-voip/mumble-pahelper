// Copyright 2005-2016 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Plugins.h"

inline QString u8(const ::std::string &str) {
	return QString::fromUtf8(str.data(), static_cast<int>(str.length()));
}

inline QString u8(const ::std::wstring &str) {
	return QString::fromStdWString(str);
}

inline ::std::string u8(const QString &str) {
	const QByteArray &qba = str.toUtf8();
	return ::std::string(qba.constData(), qba.length());
}

PluginInfo::PluginInfo() {
	locked = false;
	enabled = false;
	p = NULL;
	p2 = NULL;
}

Plugins::Plugins(QObject *p) : QObject(p) {
	// Current directory plugins
	qsCurrentDirectoryPlugins = QDir::currentPath();
	// System plugins directory
	QDir qdSystemPluginsDir_x64("C:/Program Files/Mumble/Plugins");
	QDir qdSystemPluginsDir_x86("C:/Program Files (x86)/Mumble/Plugins");
	if (qdSystemPluginsDir_x64.exists())
		qsSystemPlugins = qdSystemPluginsDir_x64.absolutePath();
	else if (qdSystemPluginsDir_x86.exists())
		qsSystemPlugins = qdSystemPluginsDir_x86.absolutePath();
	else
		qsSystemPlugins = QLatin1String("Plugins");
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
						pi->description = QString::fromStdWString(pi->p->description);
						qInfo("Description: %s", qPrintable(pi->description));
						pi->shortname = QString::fromStdWString(pi->p->shortname);
						qInfo("Shortname: %s", qPrintable(pi->shortname));
						pi->enabled = true;

						mumblePlugin2Func mpf2 = reinterpret_cast<mumblePlugin2Func>(pi->lib.resolve("getMumblePlugin2"));
						if (mpf2) {
							pi->p2 = mpf2();
							if (pi->p2->magic != MUMBLE_PLUGIN_MAGIC_2) {
								pi->p2 = NULL;
							}
						}

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

	bool ok = locked->p->fetch(fPosition, fFront, fTop, fCameraPosition, fCameraFront, fCameraTop, ssContext, swsIdentity);
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

void Plugins::on_Timer_timeout() {
	fetch();

	QReadLocker lock(&qrwlPlugins);

	if (prevlocked) {
		emit LinkLost(prevlocked);
		prevlocked = NULL;
	}

	if (! locked) {
		ssContext.clear();
		swsIdentity.clear();
	}

	std::string context;
	if (locked)
		context.assign(u8(QString::fromStdWString(locked->p->shortname)) + static_cast<char>(0) + ssContext);

	if ((context != ssContextSent) || (swsIdentity != swsIdentitySent)) {
		if (context != ssContextSent) {
			ssContextSent.assign(context);
			emit ContextChanged(u8(context));
		}
		if (swsIdentity != swsIdentitySent) {
			swsIdentitySent.assign(swsIdentity);
			emit IdentityChanged(u8(swsIdentity));
		}
	}

	if (locked) {
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
#ifdef Q_OS_WIN
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
#endif

	PluginInfo *pi = qlPlugins.at(iPluginTry);
	if (pi->enabled) {
		if (pi->p2 ? pi->p2->trylock(pids) : pi->p->trylock()) {
			pi->shortname = QString::fromStdWString(pi->p->shortname);
			emit Linked(pi);
			pi->locked = true;
			bUnlink = false;
			locked = pi;
		}
	}
}
