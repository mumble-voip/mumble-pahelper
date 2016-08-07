/* Copyright (C) 2010, Stefan Hacker <dd0t@users.sourceforge.net>

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

#include "mumblepahelper.h"
#include "ui_mumblepahelper.h"

MumblePAHelper::MumblePAHelper(QWidget *parent) :
    QMainWindow(parent) {
    plugins = new Plugins(this);
    plugins->setObjectName(QString::fromUtf8("plugins"));
    setupUi(this);
    QMainWindow::statusBar()->showMessage(tr("Not linked to plugin"));
    plugins->rescanPlugins();
}

MumblePAHelper::~MumblePAHelper() {
    if (plugins)
	delete plugins;
}

void MumblePAHelper::on_plugins_Fetched() {
    QReadLocker lock(&plugins->qrwlPlugins);

    // Output avatar posititon
    qdsbAPX->setValue(plugins->fPosition[0]);
    qdsbAPY->setValue(plugins->fPosition[1]);
    qdsbAPZ->setValue(plugins->fPosition[2]);

    qdsbAFX->setValue(plugins->fFront[0]);
    qdsbAFY->setValue(plugins->fFront[1]);
    qdsbAFZ->setValue(plugins->fFront[2]);

    qdsbATX->setValue(plugins->fTop[0]);
    qdsbATY->setValue(plugins->fTop[1]);
    qdsbATZ->setValue(plugins->fTop[2]);

    // Output camera position
    qdsbCPX->setValue(plugins->fCameraPosition[0]);
    qdsbCPY->setValue(plugins->fCameraPosition[1]);
    qdsbCPZ->setValue(plugins->fCameraPosition[2]);

    qdsbCFX->setValue(plugins->fCameraFront[0]);
    qdsbCFY->setValue(plugins->fCameraFront[1]);
    qdsbCFZ->setValue(plugins->fCameraFront[2]);

    qdsbCTX->setValue(plugins->fCameraTop[0]);
    qdsbCTY->setValue(plugins->fCameraTop[1]);
    qdsbCTZ->setValue(plugins->fCameraTop[2]);
}

void MumblePAHelper::on_plugins_IdentityChanged(const QString identity) {
    // Output identity
    qpteIdentity->setPlainText(identity);
}

void MumblePAHelper::on_plugins_ContextChanged(const QString context) {
    // Output context
    qpteContext->setPlainText(context);
}

void MumblePAHelper::on_plugins_LinkLost(const PluginInfo *pi) {
    QMainWindow::statusBar()->showMessage(tr("Lost link to plugin %1").arg(pi->shortname));

	// Reset avatar posititon
	qdsbAPX->setValue(0);
	qdsbAPY->setValue(0);
	qdsbAPZ->setValue(0);

	qdsbAFX->setValue(0);
	qdsbAFY->setValue(0);
	qdsbAFZ->setValue(0);

	qdsbATX->setValue(0);
	qdsbATY->setValue(0);
	qdsbATZ->setValue(0);

	// Reset camera position
	qdsbCPX->setValue(0);
	qdsbCPY->setValue(0);
	qdsbCPZ->setValue(0);

	qdsbCFX->setValue(0);
	qdsbCFY->setValue(0);
	qdsbCFZ->setValue(0);

	qdsbCTX->setValue(0);
	qdsbCTY->setValue(0);
	qdsbCTZ->setValue(0);
}

void MumblePAHelper::on_plugins_Linked(const PluginInfo *pi) {
    QMainWindow::statusBar()->showMessage(tr("Linked to plugin %1").arg(pi->shortname));
}

void MumblePAHelper::on_plugins_PluginList(const QList<PluginInfo*> plist) {
    qlwPlugins->clear();
    qlPluginInfo->setText(tr("No plugin selected"));
    QReadLocker lock(&plugins->qrwlPlugins);
    foreach (PluginInfo *pi, plist) {
        QListWidgetItem *item = new QListWidgetItem(pi->shortname);
        item->setData(Qt::UserRole, pi->filename);
        qlwPlugins->addItem(item);
    }
}

void MumblePAHelper::on_qpbConfig_clicked(bool) {
    QListWidgetItem *item = qlwPlugins->currentItem();
    if(item) {
	QString filename = item->data(Qt::UserRole).toString();
	QReadLocker lock(&plugins->qrwlPlugins);
	foreach (PluginInfo *pi, plugins->qlPlugins) {
	    if (pi->filename == filename) {
		lock.unlock();
		if (pi->p->config)
			pi->p->config(0);
		else {
			QMessageBox::information(this, QLatin1String("MumblePAHelper"), tr("Plugin has no configure function."), QMessageBox::Ok, QMessageBox::NoButton);
		}
		break;
	    }
	}

    }
}

void MumblePAHelper::on_qlwPlugins_currentItemChanged(QListWidgetItem *item, QListWidgetItem*) {
    if(item == NULL) {
	qlPluginInfo->setText(tr("No plugin selected"));
	return;
    }

    QString filename = item->data(Qt::UserRole).toString();
    QReadLocker lock(&plugins->qrwlPlugins);
    bool bFound = false;
    foreach (PluginInfo *pi, plugins->qlPlugins) {
	if (pi->filename == filename) {
	    qlPluginInfo->setText(tr("Filename: %1\nShortname: %2\nDescription: %3").arg(pi->filename, pi->shortname, pi->description));
	    bFound = true;
	    break;
	}
        if (!bFound){
            qlPluginInfo->setText(tr("No plugin selected"));
        }
    }
}

void MumblePAHelper::on_action_Unlink_triggered(bool) {
    plugins->bUnlink = true;
}

void MumblePAHelper::on_action_Quit_triggered(bool) {
    close();
}

void MumblePAHelper::on_action_Rescan_triggered(bool) {
    plugins->rescanPlugins();
}

void MumblePAHelper::on_action_SetSystemPluginsLocation_triggered(bool) {
    QString systemPluginsLocation = QFileDialog::getExistingDirectory(this, tr("Set system plugins directory"), plugins->qsSystemPlugins, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (systemPluginsLocation != NULL)
        plugins->qsSystemPlugins = systemPluginsLocation;
    plugins->rescanPlugins();
}

void MumblePAHelper::on_action_SetUserPluginsLocation_triggered(bool) {
    QString userPluginsLocation = QFileDialog::getExistingDirectory(this, tr("Set user plugins directory"), plugins->qsUserPlugins, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (userPluginsLocation != NULL)
        plugins->qsUserPlugins = userPluginsLocation;
    plugins->rescanPlugins();
}

void MumblePAHelper::on_qcbCurrentDirectoryPlugins_stateChanged() {
    if (qcbCurrentDirectoryPlugins->isChecked())
        plugins->bUseCurrentDirPlugins = true;
    else
        plugins->bUseCurrentDirPlugins = false;
    plugins->rescanPlugins();
}

void MumblePAHelper::on_qcbSystemPlugins_stateChanged() {
    if (qcbSystemPlugins->isChecked())
        plugins->bUseSystemPlugins = true;
    else
        plugins->bUseSystemPlugins = false;
    plugins->rescanPlugins();
}

void MumblePAHelper::on_qcbUserPlugins_stateChanged() {
    if (qcbUserPlugins->isChecked())
        plugins->bUseUserPlugins = true;
    else
        plugins->bUseUserPlugins = false;
    plugins->rescanPlugins();
}
