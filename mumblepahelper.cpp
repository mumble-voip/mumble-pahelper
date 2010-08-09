#include "mumblepahelper.h"
#include "ui_mumblepahelper.h"

MumblePAHelper::MumblePAHelper(QWidget *parent) :
    QMainWindow(parent)
{
    plugins = new Plugins(this);
    plugins->setObjectName(QString::fromUtf8("plugins"));
    setupUi(this);
    QMainWindow::statusBar()->showMessage(tr("Not linked to plugin"));
    plugins->rescanPlugins();
}

MumblePAHelper::~MumblePAHelper()
{
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
}

void MumblePAHelper::on_plugins_Linked(const PluginInfo *pi) {
    QMainWindow::statusBar()->showMessage(tr("Linked to plugin %1").arg(pi->shortname));
}

void MumblePAHelper::on_plugins_PluginList(const QList<PluginInfo*> plist) {
    qlwPlugins->clear();
    qlPluginInfo->setText(tr("No plugin selected"));
    QReadLocker lock(&plugins->qrwlPlugins);
    foreach (PluginInfo *pi, plist) {
	qlwPlugins->addItem(pi->shortname);
    }
}

void MumblePAHelper::on_qpbConfig_clicked(bool) {
    QListWidgetItem *item = qlwPlugins->currentItem();
    if(item) {
	QString name = item->text();
	QReadLocker lock(&plugins->qrwlPlugins);
	foreach (PluginInfo *pi, plugins->qlPlugins) {
	    if (pi->shortname == name) {
		lock.unlock();
		if (pi->p->config)
			pi->p->config(winId());
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

    QString name = item->text();
    QReadLocker lock(&plugins->qrwlPlugins);
    foreach (PluginInfo *pi, plugins->qlPlugins) {
	if (pi->shortname == name) {
	    qlPluginInfo->setText(tr("Filename: %1\nShortname: %2\nDescription: %3").arg(pi->filename, pi->shortname, pi->description));
	    break;
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
