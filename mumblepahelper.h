#ifndef MUMBLEPAHELPER_H
#define MUMBLEPAHELPER_H

#include <QMainWindow>
#include <QMessageBox>
#include "Plugins.h"
#include "ui_MumblePAHelper.h"


class MumblePAHelper : public QMainWindow, public Ui::MumblePAHelper
{
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

    void on_qpbConfig_clicked(bool);
    void on_qlwPlugins_currentItemChanged(QListWidgetItem*, QListWidgetItem*);

    void on_action_Unlink_triggered(bool);
    void on_action_Quit_triggered(bool);
    void on_action_Rescan_triggered(bool);
};

#endif // MUMBLEPAHELPER_H
