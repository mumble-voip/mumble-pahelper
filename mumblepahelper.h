// Copyright 2005-2016 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLEPAHELPER_H
#define MUMBLEPAHELPER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include "Plugins.h"
#include "ui_mumblepahelper.h"

class MumblePAHelper : public QMainWindow, public Ui::MumblePAHelper {
private:
	Q_OBJECT
	Q_DISABLE_COPY(MumblePAHelper)
	Plugins *plugins;
public:
	explicit MumblePAHelper(QWidget *parent = 0);
	~MumblePAHelper();

public slots:
	void on_plugins_Fetched();
	void on_plugins_IdentityChanged(const QString);
	void on_plugins_ContextChanged(const QString);
	void on_plugins_LinkLost(const PluginInfo*);
	void on_plugins_Linked(const PluginInfo*);
	void on_plugins_PluginList(const QList<PluginInfo*>);

	void on_qpbAbout_clicked(bool);
	void on_qpbConfig_clicked(bool);
	void on_qlwPlugins_currentItemChanged(QListWidgetItem*, QListWidgetItem*);

	void on_action_Unlink_triggered(bool);
	void on_action_Quit_triggered(bool);
	void on_action_Rescan_triggered(bool);

	void on_action_SetUserPluginsLocation_triggered(bool);
	void on_action_SetSystemPluginsLocation_triggered(bool);

private slots:
	void on_qcbCurrentDirectoryPlugins_stateChanged();
	void on_qcbSystemPlugins_stateChanged();
	void on_qcbUserPlugins_stateChanged();
};

#endif // MUMBLEPAHELPER_H
