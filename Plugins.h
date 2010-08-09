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
